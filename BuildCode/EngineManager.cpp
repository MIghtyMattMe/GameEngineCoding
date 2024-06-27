#include "EngineManager.h"
#include "Brush.h"
#ifdef ENGINE_CODE
#include "Inspector.h"
#else
#include "UpdateFunctions/UpdateDictionary.h"
#endif
#include "SaveLoadBuild.h"

std::vector<SDL_Scancode> KeyData::playModeInput = std::vector<SDL_Scancode>();
Uint64 KeyData::keysPressed = 0;
Uint64 KeyData::lastFrameKeys = 0;

namespace EngineManager {
    //Misc engine parts that we use
    Brush* brush = nullptr;
    bool playing = false;
    b2Vec2 cameraPos = b2Vec2(0, 0);

    //These are our gameobjects in the world
    std::vector<std::vector<GameObject*>> layeredObjectsToLoad = std::vector<std::vector<GameObject*>>();
    std::vector<std::vector<GameObject*>> layeredObjectsSaved = std::vector<std::vector<GameObject*>>();
    std::vector<GameObject*> objectsToDelete = std::vector<GameObject*>();
    GameObject* selectedObject = nullptr;
    GameObject* camFocus = nullptr;

    //physics variables and world
    b2Vec2 gravity = b2Vec2(0.0f, 10.0f);
    b2World* phyWorld;

    //gets the renderer, so fuctions do not have to constantly ask for it
    SDL_Renderer* currRenderer = nullptr;

    //Initialized the engine by creating out brush (and other stuff eventually)
    void InitEngine(SDL_Renderer* renderer) {
        brush = new Brush(renderer);
        currRenderer = renderer;
        for (int i = 0; i < 8; i++) {
            layeredObjectsToLoad.push_back(std::vector<GameObject*>());
            layeredObjectsSaved.push_back(std::vector<GameObject*>());
        }
        UpdateDictionary::Generate();
#ifndef ENGINE_CODE
        SaveLoadBuild::LoadFile("main.smol", currRenderer, layeredObjectsToLoad, brush);
#endif
    }
    //remove all our allocated memory
    void CloseEngine() {
        delete brush;
        brush = NULL;
        for (std::vector<GameObject*> &layer : layeredObjectsToLoad) {
            for (GameObject* &gObj : layer) {
                delete gObj;
            }
            layer.clear();
        }
        for (std::vector<GameObject*> &layer : layeredObjectsSaved) {
            for (GameObject* &gObj : layer) {
                delete gObj;
            }
            layer.clear();
        }
        //selectedObj is still in list of layeredObjectsToLoad, so it already
        //was deleted, but we still need to nullify the pointer
        selectedObject = NULL;
        currRenderer = NULL;
    }

    void PlayEngine() {
        selectedObject = nullptr;
        for (Uint8 i = 0; i < layeredObjectsToLoad.size(); i++) {
            //copy gameObjects to saved list
            for (GameObject* &gObj : layeredObjectsToLoad[i]) {
                layeredObjectsSaved[i].push_back(gObj->Clone(currRenderer));
                gObj->CreateAndPlaceBody(phyWorld);
            }
        }
        playing = true;
    }

    void StopEngine() {
        if (camFocus) camFocus = camFocus->Clone(currRenderer);
        selectedObject = nullptr;
        for (Uint8 i = 0; i < layeredObjectsToLoad.size(); i++) {
            //destroy our game world
            for (GameObject* &gObj : layeredObjectsToLoad[i]) {
                if (gObj->objBody != nullptr) phyWorld->DestroyBody(gObj->objBody);
                delete gObj; //This delete's the gameObjects
            }
            layeredObjectsToLoad[i].clear(); //This delete's the pointers

            //rebuild the world using saved objects, then clear the saved list
            for (GameObject* &gObj : layeredObjectsSaved[i]) {
                if (camFocus && *gObj == *camFocus) {
                    layeredObjectsToLoad[i].push_back(camFocus);
                } else {
                    GameObject *newObj = gObj->Clone(currRenderer);
                    layeredObjectsToLoad[i].push_back(newObj);
                }
                delete gObj;
            }
            layeredObjectsSaved[i].clear();
        }
    }

#ifdef ENGINE_CODE
    //Main Render Loop for our engine interface
    void RenderEngine() {
        MakeTools();
        MakeInspector();
        MakeControlBoard();
    }

    //makes the tool window and sets up its logic
    void MakeTools() {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 1));
        ImGui::Begin("EngineTools");
        ImGui::PopStyleColor();
        if (ImGui::Button("Circle")) {
            brush->setType(Brush::Type::Circle);
        }
        ImGui::SameLine();
        if (ImGui::Button("Square")) {
            brush->setType(Brush::Type::Square);
        }
        if (ImGui::Button("Triangle")) {
            brush->setType(Brush::Type::Triangle);
        }
        ImGui::SameLine();
        if (ImGui::Button("Custom")) {
            brush->setType(Brush::Type::Custom);
        }
        ImGui::End();
    }
    //creates the inspector window and sets up its logic
    void MakeInspector() {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 1));
        ImGui::Begin("Object Inspector");
        ImGui::PopStyleColor();
        if (selectedObject) {
            //display data if we are playing, but if not, data is editable
            if (playing) {
                Inspector::BuildPlayModeInspector(selectedObject);
            } else {
                bool isCamFocusSelected = selectedObject == camFocus;
                if (ImGui::Checkbox("Camera Focus", &isCamFocusSelected)) {
                    if (isCamFocusSelected) { 
                        camFocus = selectedObject; 
                    } else {
                        camFocus = nullptr;
                    }
                }
                Inspector::BuildNormalInspector(selectedObject, &layeredObjectsToLoad[0], currRenderer);
                if (ImGui::Button("Delete")) {
                    for (size_t i = 0; i < layeredObjectsToLoad[selectedObject->layer].size(); i++) {
                        if (layeredObjectsToLoad[selectedObject->layer][i] == selectedObject) {
                            if (camFocus && *selectedObject == *camFocus) camFocus = nullptr;
                            layeredObjectsToLoad[selectedObject->layer].erase(layeredObjectsToLoad[selectedObject->layer].begin() + i);
                            delete selectedObject;
                            selectedObject = nullptr;
                            break;
                        }
                    }
                }
            }
        } else {
            ImGui::Text("No selected object!");
            ImGui::Text("Right-Click on an object.");
        }
        ImGui::End();
    }
    //makes the control board for running the game and editing configurations
    void MakeControlBoard() {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 1));
        ImGui::Begin("Control Board");
        ImGui::PopStyleColor();

        //Handles loading and saving the game engine
        if (ImGui::Button("Save") && !playing) {
            std::string savePath = SaveLoadBuild::CreateSaveDialogBox();
            if (!savePath.empty()) {
                if (!SaveLoadBuild::SaveFile(savePath, currRenderer, layeredObjectsToLoad)) {
                    SDL_Log("Failed to Save");
                }
            } else {
                SDL_Log("Failed to find Save File");
            }
        }
        ImGui::SameLine();
        //when Stop is hit, read all gameObjects from saved vector and delete memory
        if (ImGui::Button("Load") && !playing) {
            selectedObject = nullptr;
            std::string loadPath = SaveLoadBuild::CreateLoadDialogBox();
            if (!loadPath.empty()) {
                if (!SaveLoadBuild::LoadFile(loadPath, currRenderer, layeredObjectsToLoad, brush)) {
                    SDL_Log("Failed to Load");
                }
            } else {
                SDL_Log("Failed to find Load File");
            }
        }

        //Handles the play and Stop functionality
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.7f, 1));
        if (!playing) ImGui::PopStyleColor();
        bool playJustHit = false;
        //when Play is hit, write all gameObjects to a vector
        if (ImGui::Button("Play")) {
            selectedObject = nullptr;
            if (!playing) {
                playJustHit = true;
                PlayEngine();
            }
            playing = true;
        }
        if (playing && !playJustHit) ImGui::PopStyleColor();
        ImGui::SameLine();
        //when Stop is hit, read all gameObjects from saved vector and delete memory
        if (ImGui::Button("Stop")) {
            selectedObject = nullptr;
            if (playing) {
                StopEngine();
            }
            playing = false;
        }
        
        //handles building the game
        if (ImGui::Button("Build")) {
            std::string buildPath = SaveLoadBuild::GetFolderDialogBox();
            std::string buildPathAndFile[2] = {};
            size_t lastSlash = buildPath.find_last_of('\\');
            buildPathAndFile[0] = buildPath.substr(0, lastSlash);
            buildPathAndFile[1] = buildPath.substr(lastSlash + 1);
            SaveLoadBuild::BuildGame(buildPathAndFile[0], buildPathAndFile[1], currRenderer, layeredObjectsToLoad);
        }

        //Regenerate the Update functions
        if (ImGui::Button("Generate Function List")) {
            UpdateDictionary::Generate();
        }
        ImGui::End();
    }

    //Logic for Selection of GameObjects
    void SelectObject(GameObject* clickedObj) {
        selectedObject = clickedObj;
    }
    GameObject* FindSelectedObject(b2Vec2 clickedPos) {
        clickedPos += cameraPos;
        for (int l = ((int)layeredObjectsToLoad.size() - 1); l >= 0; l--) {
            std::vector<GameObject*> &layer = layeredObjectsToLoad[l];
            for (GameObject* &gObj : layer) {
                if (gObj->objShape == GameObject::Polygon) {
                    //check the verts
                    const int numVerts = (int) gObj->verts.size();
                    b2Vec2 bodyPosition = (playing) ? gObj->objBody->GetPosition() : gObj->objBodyDef.position;
                    int intersections = 0;
                    for (Uint8 i = 0; i < numVerts; i++) {
                        b2Vec2 vert1 = b2Vec2(((gObj->verts[i].x * gObj->width) + bodyPosition.x), ((gObj->verts[i].y * gObj->height) + bodyPosition.y));
                        b2Vec2 vert2 = b2Vec2(((gObj->verts[(i + 1) % numVerts].x * gObj->width) + bodyPosition.x), ((gObj->verts[(i + 1) % numVerts].y * gObj->height) + bodyPosition.y));
                        b2Vec2 leftRay = b2Vec2(clickedPos.x - 100, clickedPos.y);
                        bool a = ((leftRay.y - vert1.y) * (clickedPos.x - vert1.x)) > ((clickedPos.y - vert1.y) * (leftRay.x - vert1.x));
                        bool b = ((leftRay.y - vert2.y) * (clickedPos.x - vert2.x)) > ((clickedPos.y - vert2.y) * (leftRay.x - vert2.x));
                        bool c = ((vert2.y - vert1.y) * (clickedPos.x - vert1.x)) > ((clickedPos.y - vert1.y) * (vert2.x - vert1.x));
                        bool d = ((vert2.y - vert1.y) * (leftRay.x - vert1.x)) > ((leftRay.y - vert1.y) * (vert2.x - vert1.x));
                        if (a != b && c != d) {
                            intersections++;
                        }
                    }
                    if (intersections % 2) {
                        return gObj;
                    }
                    
                } else {
                    //find if the clicked position is on the object's rectangle
                    float leftLimit = (playing) ? gObj->objBody->GetPosition().x - (gObj->width / 2) : gObj->objBodyDef.position.x - (gObj->width / 2);
                    float rightLimit = (playing) ? gObj->objBody->GetPosition().x + (gObj->width / 2) : gObj->objBodyDef.position.x + (gObj->width / 2);
                    float upLimit = (playing) ? gObj->objBody->GetPosition().y - (gObj->height / 2) : gObj->objBodyDef.position.y - (gObj->height / 2);
                    float downLimit = (playing) ? gObj->objBody->GetPosition().y + (gObj->height / 2) : gObj->objBodyDef.position.y + (gObj->height / 2);
                    if (clickedPos.x < rightLimit && 
                    clickedPos.x > leftLimit &&
                    clickedPos.y < downLimit &&
                    clickedPos.y > upLimit) {
                        return gObj;
                    }
                }
            }
        }
        return nullptr;
    }
#endif

    //These take an existing gameObject (or create a new one) and adds it to the list of objects to be rendered
    void AddToViewPort(GameObject* gameObject) {
        layeredObjectsToLoad[gameObject->layer].push_back(gameObject);
    }
    void AddToViewPort(b2Vec2 targetPos, float targetWidth, float targetHeight, std::string textureFile) {
        GameObject* newObj = nullptr;
        b2BodyDef newBody;
        newBody.position.Set(targetPos.x + cameraPos.x, targetPos.y + cameraPos.y);
        newBody.type = b2_staticBody;
        if (textureFile.empty()) {
            if (brush->getTextureFile().empty()) {
                SDL_Log("No brush texture was chosen!");
                return;
            }
            newObj = new GameObject(currRenderer, newBody, brush->getType(), targetWidth, targetHeight, brush->texturesSaved[brush->getType()]);
            if (brush->getType() == Brush::Player) {
                newObj->objBodyDef.type = b2_dynamicBody;
                newObj->objBodyDef.fixedRotation = true;
            }
        } else {
            newObj = new GameObject(currRenderer, newBody, 0, targetWidth, targetHeight, brush->texturesSaved[2]);
        }
        if (playing) {
            newObj->CreateAndPlaceBody(phyWorld);
        }
        layeredObjectsToLoad[newObj->layer].push_back(newObj);
    }
    //handles the actual drawing of textures
    void DrawViewPort() {
        //go over every object, make a rectangle, and apply the texture
        int j = 0;
        for (std::vector<GameObject*> &layer : layeredObjectsToLoad) {
            for (size_t i = 0; i < layer.size(); i++) {
                GameObject* gObj = layer[i];

                //for polygons, get verts -> translate based on position and width -> translate based on rotation -> convert to pixel space
                //Apply position to SDL_Vertex -> apply color to SDL_Vertex -> SDL_RenderGeometry()
                if (gObj->objShape == GameObject::Polygon) {
                    SDL_Vertex geoVerts[8];
                    const int numVerts = (int)gObj->verts.size();
                    b2Vec2 bodyPosition = (playing) ? gObj->objBody->GetPosition() : gObj->objBodyDef.position;
                    float rotation = (playing) ? gObj->objBody->GetAngle() : gObj->objBodyDef.angle;
                    SDL_FPoint centroid = SDL_FPoint(0, 0);
                    for (Uint8 i = 0; i < numVerts; i++) {
                        float posX = gObj->verts[i].x * gObj->width;
                        float posY = gObj->verts[i].y * gObj->height;
                        float posXRot = (posX * cos(rotation) - posY * sin(rotation)) + bodyPosition.x - cameraPos.x;
                        float posYRot = (posX * sin(rotation) + posY * cos(rotation)) + bodyPosition.y - cameraPos.y;
                        geoVerts[i].position.x = MeterToPixel(posXRot);
                        geoVerts[i].position.y = MeterToPixel(posYRot);
                        geoVerts[i].color.r = gObj->color.r / 255.0f;
                        geoVerts[i].color.g = gObj->color.g / 255.0f;
                        geoVerts[i].color.b = gObj->color.b / 255.0f;
                        geoVerts[i].color.a = gObj->color.a / 255.0f;
                        centroid.x += posXRot;
                        centroid.y += posYRot;
                    }
                    centroid.x = MeterToPixel(centroid.x / numVerts);
                    centroid.y = MeterToPixel(centroid.y / numVerts);
                    //map points to various triangles, centered at 0,0
                    SDL_Vertex center = SDL_Vertex();
                    center.position = centroid;
                    center.color.r = gObj->color.r / 255.0f;
                    center.color.g = gObj->color.g / 255.0f;
                    center.color.b = gObj->color.b / 255.0f;
                    center.color.a = gObj->color.a / 255.0f;
                    std::vector<SDL_Vertex> geoPoints = std::vector<SDL_Vertex>();
                    for (Uint8 i = 0; i < numVerts; i++) {
                        geoPoints.push_back(geoVerts[i]);
                        geoPoints.push_back(center);
                        geoPoints.push_back(geoVerts[(i + 1) % numVerts]);
                    }
                    //does not alpha, and does not save nor reload on "save/load", and no texture
                    SDL_SetTextureBlendMode(gObj->GetTexturePtr(), SDL_BLENDMODE_BLEND);
                    SDL_RenderGeometry(currRenderer, NULL, geoPoints.data(), (int) geoPoints.size(), NULL, 0);
                } else {
                    SDL_FRect texture_rect;
                    texture_rect.x = (playing) ? MeterToPixel(gObj->objBody->GetPosition().x - (gObj->width / 2) - cameraPos.x) : MeterToPixel(gObj->objBodyDef.position.x - (gObj->width / 2) - cameraPos.x);
                    texture_rect.y = (playing) ? MeterToPixel(gObj->objBody->GetPosition().y - (gObj->height / 2) - cameraPos.y) : MeterToPixel(gObj->objBodyDef.position.y - (gObj->height / 2) - cameraPos.y);
                    texture_rect.w = MeterToPixel(gObj->width);
                    texture_rect.h = MeterToPixel(gObj->height);
                    float rotation = (playing) ? gObj->objBody->GetAngle() : gObj->objBodyDef.angle;
                    rotation *= (180 /b2_pi);
                    SDL_SetTextureColorMod(gObj->GetTexturePtr(), gObj->color.r, gObj->color.g, gObj->color.b);
                    SDL_SetTextureAlphaMod(gObj->GetTexturePtr(), gObj->color.a);
                    if (SDL_RenderTextureRotated(currRenderer, gObj->GetTexturePtr(), NULL, &texture_rect, rotation, NULL, SDL_FlipMode::SDL_FLIP_NONE)) {
                        std::cout << SDL_GetError() << " with " << gObj->GetTexturePtr() << std::endl;
                    }
                }
            }
            j++;
        }
    }

    //Runs the Update Loop on gameObjects when playing
    void UpdateGameObjects() {
        if (!playing) return;
        float timeStep = 1.0f / 300.0f;
        phyWorld->Step(timeStep, 6, 2);
        for (std::vector<GameObject*> &layer : layeredObjectsToLoad) {
            for (GameObject* &gObj : layer) {
                CoreUpdateFunctions::grounded = false;
                CoreUpdateFunctions::key = false;
                CoreUpdateFunctions::touch = false;
                for (auto func : UpdateDictionary::UpdateFunctions[gObj->UpdateFunction]) {
                    if (gObj == nullptr) break;
                    (func.function)(func.conditional, func.notted, gObj, func.data);
                }
            }
        }
    }
    void ReadPlayInput(SDL_Event event) {
        if (!playing) return;
        SDL_Scancode code = event.key.keysym.scancode;
        if (code < 64) {
            KeyData::playModeInput.push_back(code);
            if (event.type == SDL_EVENT_KEY_DOWN) {
                KeyData::keysPressed |= (1ULL << code);
            } else if (event.type == SDL_EVENT_KEY_UP) {
                KeyData::keysPressed ^= (1ULL << code);
            }
        }
    }
    GameObject* FindObjectFromBody(b2Body &bodyToFind) {
        for (GameObject* gObj : layeredObjectsToLoad[(int)log2(bodyToFind.GetFixtureList()[0].GetFilterData().maskBits)]) {
            if (gObj->objBody->GetPosition() == bodyToFind.GetPosition()) {
                return gObj;
            }
        }
        return nullptr;
    }
    void QueueObjectToDestroy(GameObject *objectToDestroy) {
        for (GameObject *gObj : objectsToDelete) {
            if (*gObj == *objectToDestroy) return;
        }
        objectsToDelete.push_back(objectToDestroy);
    }
    void DestroyObjects() {
        while (objectsToDelete.size() > 0) {
            for (size_t i = 0; i < layeredObjectsToLoad[objectsToDelete[0]->layer].size(); i++) {
                if (layeredObjectsToLoad[objectsToDelete[0]->layer][i] == objectsToDelete[0]) {
                    if (*camFocus == *objectsToDelete[0]) camFocus = nullptr;
                    layeredObjectsToLoad[objectsToDelete[0]->layer].erase(layeredObjectsToLoad[objectsToDelete[0]->layer].begin() + i);
                    phyWorld->DestroyBody(objectsToDelete[0]->objBody);
                    delete objectsToDelete[0];
                    objectsToDelete[0] = nullptr;
                    objectsToDelete.erase(objectsToDelete.begin());
                    break;
                }
            }
        }
    }

    //Controlling the Camera offset and focus
    b2Vec2 GetCameraPosition() {
        return cameraPos;
    }
    void SetCameraPosition(b2Vec2 newPos) {
        cameraPos = newPos;
    }
    void SetCamFocus(GameObject* newFocus) {
        camFocus = newFocus;
    }
    GameObject* GetCamFocus() {
        return camFocus;
    }
    void FocusCamera() {
        if (playing && camFocus) {
            b2Vec2 newPos = VectorMeterToPixel(camFocus->objBody->GetPosition());
            newPos.x -= 640;
            newPos.y -= 360;
            SetCameraPosition(VectorPixelToMeter(newPos));
        }
    }

    //these are used for physics world creation in main.cpp
    b2Vec2 GetGravityVector() {
        return gravity;
    }
    void SetPhyWorld(b2World* world) {
        phyWorld = world;
    }

    float MeterToPixel(float meters) { return (meters * 50); }
    float PixToMeter(float pixels) { return (pixels * 0.02f); }
    b2Vec2 VectorMeterToPixel(b2Vec2 meterVec) { return b2Vec2(meterVec.x * 50, meterVec.y * 50); }
    b2Vec2 VectorPixelToMeter(b2Vec2 pixelVec) { return b2Vec2(pixelVec.x * 0.02f, pixelVec.y * 0.02f); }
}