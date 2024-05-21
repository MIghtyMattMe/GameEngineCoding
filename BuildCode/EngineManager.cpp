#include "EngineManager.h"
#include "Brush.h"
#include "SaveLoadBuild.h"
#include "UpdateFunctions/UpdateDictionary.h"

std::vector<SDL_Scancode> KeyData::playModeInput = std::vector<SDL_Scancode>();
Uint64 KeyData::keysPressed = 0;
Uint64 KeyData::lastFrameKeys = 0;

namespace EngineManager {
    //Misc engine parts that we use
    Brush* brush = nullptr;
    bool playing = false;
    b2Vec2 cameraPos = b2Vec2(0, 0);

    //These are our gameobjects in the world
    //std::vector<GameObject*> objectsToLoad = std::vector<GameObject*>();
    std::vector<std::vector<GameObject*>> layeredObjectsToLoad = std::vector<std::vector<GameObject*>>();
    std::vector<std::vector<GameObject*>> layeredObjectsSaved = std::vector<std::vector<GameObject*>>();
    //std::vector<GameObject*> savedObjects = std::vector<GameObject*>();
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
        for (int i = 0; i < 8; i++) {
            layeredObjectsToLoad.push_back(std::vector<GameObject*>());
            layeredObjectsSaved.push_back(std::vector<GameObject*>());
        }
        UpdateDictionary::Generate();
        SaveLoadBuild::LoadFile("main.smol", currRenderer, layeredObjectsToLoad);
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
        for (Uint8 i = 0; i < layeredObjectsToLoad.size(); i++) {
            for (GameObject* &gObj : layeredObjectsToLoad[i]) {
                layeredObjectsSaved[i].push_back(gObj->Clone(currRenderer));
                gObj->CreateAndPlaceBody(phyWorld);
            }
        }
        playing = true;
    }

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
            newObj = new GameObject(currRenderer, newBody, brush->getType(), targetWidth, targetHeight, brush->getTextureFile());
            if (brush->getType() == Brush::Player) {
                //newObj->SetUpdateFunction(DefaultUpdates::MovePlayer);
                newObj->objBodyDef.type = b2_dynamicBody;
                newObj->objBodyDef.fixedRotation = true;
            }
        } else {
            newObj = new GameObject(currRenderer, newBody, 0, targetWidth, targetHeight, textureFile);
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
                    const int numVerts = gObj->verts.size();
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
                    SDL_RenderGeometry(currRenderer, NULL, geoPoints.data(), geoPoints.size(), NULL, 0);
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
                    SDL_RenderTextureRotated(currRenderer, gObj->GetTexturePtr(), NULL, &texture_rect, rotation, NULL, SDL_FlipMode::SDL_FLIP_NONE);
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
                for (auto func : UpdateDictionary::UpdateFunctions[gObj->UpdateFunction]) {
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
}