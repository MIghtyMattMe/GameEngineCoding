//Custom scritps
#include "EngineManager.h"
#include "Brush.h"
#include "GameObject.h"
#include "Inspector.h"

//SDL and ImGui database
#include "imgui/imgui.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_image.h"

//Box2D is the physics library
#include "box2d/box2d.h"

//used for getting files from the user
#include <ShObjIdl_core.h>

//these are used for debugging and saving/loading files
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

namespace EngineManager {
    //Misc engine parts that we use
    Brush* brush = nullptr;
    bool playing = false;
    b2Vec2 cameraPos = b2Vec2(0, 0);

    //These are our gameobjects in the world
    std::vector<GameObject*> objectsToLoad = std::vector<GameObject*>();
    std::vector<GameObject*> savedObjects = std::vector<GameObject*>();
    GameObject* selectedObject = nullptr;

    //physics variables and world
    b2Vec2 gravity = b2Vec2(0.0f, 10.0f);
    b2World* phyWorld;

    //gets the renderer, so fuctions do not have to constantly ask for it
    SDL_Renderer* currRenderer = nullptr;

    //Initialized the engine by creating out brush (and other stuff eventually)
    void InitEngine(SDL_Renderer* renderer) {
        brush = new Brush();
        currRenderer = renderer;
    }
    //remove all our allocated memory
    void CloseEngine() {
        delete brush;
        brush = NULL;
        for (GameObject* gObj : objectsToLoad) {
            delete gObj;
        }
        objectsToLoad.clear();
        for (GameObject* gObj : savedObjects) {
            delete gObj;
        }
        savedObjects.clear();
        //selectedObj is still in list of objectsToLoad, so it already
        //was deleted, but we still need to nullify the pointer
        selectedObject = NULL;
        currRenderer = NULL;
    }

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
        ImGui::End();
    }
    //creates the inspector window and sets up its logic
    void MakeInspector() {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 1));
        ImGui::Begin("Object Inspector");
        ImGui::PopStyleColor();
        if (selectedObject != nullptr) {
            //display data
            float xPos = selectedObject->objBodyDef.position.x;
            float yPos = selectedObject->objBodyDef.position.y;
            ImGui::SliderFloat("X Pos", &xPos, 0, 50);
            ImGui::SliderFloat("Y Pos", &yPos, 0, 20);
            ImGui::SliderFloat("Width", &selectedObject->width, 0.1f, 30);
            ImGui::SliderFloat("Height", &selectedObject->height, 0.1f, 30);
            selectedObject->objBodyDef.position.Set(xPos, yPos);

            Inspector::BuildPhysicsBodySelector(selectedObject);
            Inspector::BuildShapeSelector(selectedObject);

            if (ImGui::Button("Delete")) {
                for (size_t i = 0; i < objectsToLoad.size(); i++) {
                    if (*objectsToLoad[i] == *selectedObject) {
                        objectsToLoad.erase(objectsToLoad.begin() + i);
                        delete selectedObject;
                        selectedObject = NULL;
                        break;
                    }
                }
            }
        } else {
            ImGui::Text("No selected object!");
            ImGui::Text("Right-Click on an object to select it.");
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
            SDL_Log("Save dialog");
            std::string savePath = CreateSaveDialogBox();
            if (!savePath.empty()) {
                SDL_Log("Save file");
                if (!SaveFile(savePath)) {
                    SDL_Log("Failed to Save");
                }
            } else {
                SDL_Log("Failed to find Save File");
            }
        }
        ImGui::SameLine();
        //when Stop is hit, read all gameObjects from saved vector and delete memory
        if (ImGui::Button("Load") && !playing) {
            std::string loadPath = CreateLoadDialogBox();
            if (!loadPath.empty()) {
                if (!LoadFile(loadPath)) {
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
                for (GameObject* gObj : objectsToLoad) {
                    b2BodyDef newBody;
                    newBody.position.Set(gObj->objBodyDef.position.x, gObj->objBodyDef.position.y);
                    newBody.type = gObj->objBodyDef.type;
                    GameObject* newObj = new GameObject(currRenderer, newBody, gObj->objShape, gObj->width, gObj->height, gObj->GetFilePath());
                    newObj->SetTextureFromeFile(currRenderer, newObj->GetFilePath());
                    savedObjects.push_back(newObj);

                    if (gObj->objShape == GameObject::box) {
                        gObj->objBody = phyWorld->CreateBody(&gObj->objBodyDef);
                        b2PolygonShape box;
                        box.SetAsBox(gObj->width / 2, gObj->height / 2);
                        b2FixtureDef boxFixtureDef;
                        boxFixtureDef.shape = &box;
                        boxFixtureDef.density = 1.0f;
                        boxFixtureDef.friction = 0.3f;
                        gObj->objBody->CreateFixture(&boxFixtureDef);
                    } else if (gObj->objShape == GameObject::ecllips) {
                        const int STEPS = 32;
                        float a = (gObj->width >= gObj->height) ? gObj->width / 2 : gObj->height / 2;
                        float b = (gObj->width >= gObj->height) ? gObj->height / 2 : gObj->width / 2;
                        b2Vec2 verts[STEPS];
                        for (int i = 0; i < STEPS; i++) {
                            float t = (float) (i * 2 * b2_pi) / STEPS;
                            verts[i] = b2Vec2((gObj->width / 2) * cosf(t), (gObj->height / 2) * sinf(t));
                        }
                        gObj->objBody = phyWorld->CreateBody(&gObj->objBodyDef);

                        //circle made for normal collisions
                        b2CircleShape cir;
                        cir.m_radius = b;
                        b2FixtureDef FixtureDef;
                        FixtureDef.shape = &cir;
                        FixtureDef.density = 1.0f;
                        gObj->objBody->CreateFixture(&FixtureDef);

                        //chain made for ellipse collisions
                        b2ChainShape chainShape;
                        chainShape.CreateLoop(verts, STEPS);
                        b2FixtureDef triFixtureDef;
                        triFixtureDef.shape = &chainShape;
                        triFixtureDef.density = 1.0f;
                        gObj->objBody->CreateFixture(&triFixtureDef);
                    } else if (gObj->objShape == GameObject::polygon) {
                        b2Vec2 points[3] = {b2Vec2(0, -0.5f), b2Vec2(0.5f, 0.5f), b2Vec2(-0.5f, 0.5f)};
                        for (int i = 0; i < 3; i++) {
                            points[i].x *= gObj->width;
                            points[i].y *= gObj->height;
                        }
                        gObj->objBody = phyWorld->CreateBody(&gObj->objBodyDef);
                        b2PolygonShape polygon;
                        polygon.Set(points, 3);
                        b2FixtureDef polygonFixtureDef;
                        polygonFixtureDef.shape = &polygon;
                        polygonFixtureDef.density = 1.0f;
                        polygonFixtureDef.friction = 0.3f;
                        gObj->objBody->CreateFixture(&polygonFixtureDef);
                    }
                }
            }
            playing = true;
        }
        if (playing && !playJustHit) ImGui::PopStyleColor();
        ImGui::SameLine();
        //when Stop is hit, read all gameObjects from saved vector and delete memory
        if (ImGui::Button("Stop")) {
            selectedObject = nullptr;
            if (playing) {
                for (GameObject* gObj : objectsToLoad) {
                    if (gObj->objBody != nullptr) phyWorld->DestroyBody(gObj->objBody);
                    delete gObj; //This delete's the gomeObjects
                }
                objectsToLoad.clear(); //This delete's the pointers
                for (GameObject* gObj : savedObjects) {
                    b2BodyDef newBody;
                    newBody.position.Set(gObj->objBodyDef.position.x, gObj->objBodyDef.position.y);
                    newBody.type = gObj->objBodyDef.type;
                    GameObject* newObj = new GameObject(currRenderer, newBody, gObj->objShape, gObj->width, gObj->height, gObj->GetFilePath());
                    newObj->SetTextureFromeFile(currRenderer, newObj->GetFilePath());
                    objectsToLoad.push_back(newObj);
                    delete gObj;
                }
                savedObjects.clear();
            }
            playing = false;
        }
        ImGui::End();
    }
    
    //Logic for Selection of GameObjects
    void SelectObject(GameObject* clickedObj) {
        selectedObject = clickedObj;
    }
    GameObject* FindSelectedObject(b2Vec2 clickedPos) {
        clickedPos += cameraPos;
        for (GameObject* gObj : objectsToLoad) {
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
        return nullptr;
    }

    //These take an existing gameObject (or create a new one) and adds it to the list of objects to be rendered
    void AddToViewPort(GameObject* gameObject) {
        objectsToLoad.push_back(gameObject);
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
            newObj = new GameObject(currRenderer, newBody, brush->getType(), targetWidth, targetHeight, brush->getTextureFile());
        } else {
            newObj = new GameObject(currRenderer, newBody, 0, targetWidth, targetHeight, textureFile);
        }
        objectsToLoad.push_back(newObj);
    }
    //handles the actual drawing of textures
    void DrawViewPort() {
        //go over every object, make a rectangle, and apply the texture
        for (GameObject* gObj : objectsToLoad) {
            SDL_FRect texture_rect;
            texture_rect.x = (playing) ? MeterToPixel(gObj->objBody->GetPosition().x - (gObj->width / 2) - cameraPos.x) : MeterToPixel(gObj->objBodyDef.position.x - (gObj->width / 2) - cameraPos.x);
            texture_rect.y = (playing) ? MeterToPixel(gObj->objBody->GetPosition().y - (gObj->height / 2) - cameraPos.y) : MeterToPixel(gObj->objBodyDef.position.y - (gObj->height / 2) - cameraPos.y);
            texture_rect.w = MeterToPixel(gObj->width);
            texture_rect.h = MeterToPixel(gObj->height);
            float rotation = (playing) ? gObj->objBody->GetAngle() : gObj->objBodyDef.angle;
            rotation *= (180 /b2_pi);
            //std::cout << std::to_string(rotation) << std::endl;
            SDL_FPoint center = SDL_FPoint(0, 0);
            SDL_RenderTextureRotated(currRenderer, gObj->GetTexturePtr(), NULL, &texture_rect, rotation, NULL, SDL_FlipMode::SDL_FLIP_NONE);
        }
    }

    //Runs the Update Loop on gameObjects when playing
    void UpdateGameObjects() {
        if (!playing) return;
        float timeStep = 1.0f / 300.0f;
        phyWorld->Step(timeStep, 6, 2);
        for (GameObject* gObj : objectsToLoad) {
            gObj->Update();
        }
    }

    //Controlling the Camera offset
    b2Vec2 GetCameraPosition() {
        return cameraPos;
    }
    void SetCameraPosition(b2Vec2 newPos) {
        cameraPos = newPos;
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

    //These are all the functions that handle saving and loading files in/out of the engine
    bool LoadFile(std::string path) {
        //first, load the file
        std::ifstream srcFile(path);
        std::string input;
        //then delete the current game
        for (GameObject* gObj : objectsToLoad) {
            delete gObj;
        }
        objectsToLoad.clear();
        //then, read new gameObjects from the file
        while (std::getline(srcFile, input)) {
            size_t lineIndex = 0;
            std::string gObjPiece = "";
            std::vector<std::string> gObjPieces = std::vector<std::string>();
            while ((lineIndex = input.find(':')) != std::string::npos) {
                gObjPiece = input.substr(0, input.find(':'));
                gObjPieces.push_back(gObjPiece);
                input.erase(0, lineIndex + 1);
            }
            //this assumes that the last item of the saved gameobj is always the texture path
            AddToViewPort(b2Vec2(std::stof(gObjPieces[0]), std::stof(gObjPieces[1])), std::stof(gObjPieces[6]), std::stof(gObjPieces[7]), input);
        }
        srcFile.close();
        return true;
    }
    bool SaveFile(std::string path) {
        //write all the gameObjects to the file, each terminated by new line (\n)
        std::ofstream dstFile(path, std::ofstream::trunc);
        std::string newLine = "";
        for (GameObject* gObj : objectsToLoad) {
            newLine += std::to_string(gObj->objBodyDef.position.x) + ":";
            newLine += std::to_string(gObj->objBodyDef.position.y) + ":";
            newLine += std::to_string(gObj->color.r) + ":";
            newLine += std::to_string(gObj->color.g) + ":";
            newLine += std::to_string(gObj->color.b) + ":";
            newLine += std::to_string(gObj->color.a) + ":";
            newLine += std::to_string(gObj->width) + ":";
            newLine += std::to_string(gObj->height) + ":";
            newLine += gObj->GetFilePath();
            newLine += "\n";
            dstFile << newLine;
            newLine = "";
        }
        dstFile.close();
        return true;
    }
    //Code largely copied from post: https://cplusplus.com/forum/windows/275617/ made by freddie1
    //This opens the Windows file explorer boxes for users to navigate
    std::string CreateLoadDialogBox() {
        COMDLG_FILTERSPEC ComDlgFS[1] = {{L"Smol GameEngine (*.smol)", L"*.smol"}};
        IFileOpenDialog* pFileOpen = nullptr;
        IShellItem* pShellItem = nullptr;
        wchar_t* ppszName = nullptr;
        std::string finalPath = "";

        CoInitialize(NULL);
        CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_IFileOpenDialog, (void**)(&pFileOpen));
        pFileOpen->SetFileTypes(1, ComDlgFS);
        pFileOpen->Show(0);
        pFileOpen->GetResult(&pShellItem);
        if (pShellItem != nullptr) {
            pShellItem->GetDisplayName(SIGDN_FILESYSPATH,&ppszName);
            std::wstring ws(ppszName);
            std::string filePath(ws.begin(), ws.end());
            finalPath = filePath;
            pShellItem->Release();
        }
        pFileOpen->Release();    
        CoUninitialize();
        return finalPath;
    }
    std::string CreateSaveDialogBox() {
        COMDLG_FILTERSPEC ComDlgFS[1] = {{L"Smol GameEngine (*.smol)", L"*.smol"}};
        IFileSaveDialog* pFileSave = nullptr;
        IShellItem* pShellItem = nullptr;
        wchar_t* ppszName = nullptr;
        std::string finalPath = "";

        CoInitialize(NULL);
        CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_IFileSaveDialog, (void**)(&pFileSave));
        pFileSave->SetFileTypes(1, ComDlgFS);
        pFileSave->Show(0);
        pFileSave->GetResult(&pShellItem);
        if (pShellItem != nullptr) {
            pShellItem->GetDisplayName(SIGDN_FILESYSPATH,&ppszName);
            std::wstring ws(ppszName);
            std::string filePath(ws.begin(), ws.end());
            std::transform(ws.begin(), ws.end(), std::back_inserter(filePath), [] (wchar_t c) {
                return (char)c;
            });
            if (filePath.substr(filePath.length() - 5) != ".smol") filePath += ".smol";
            std::ofstream dstFile(filePath);
            finalPath = filePath;
            dstFile.close();
            pShellItem->Release();
        }
        pFileSave->Release();
        CoUninitialize();
        return finalPath;
    }
}