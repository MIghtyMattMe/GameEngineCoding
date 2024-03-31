#include "EngineManager.h"
#include "Brush.h"
#include "Inspector.h"

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
        if (ImGui::Button("Player")) {
            brush->setType(Brush::Type::Player);
        }
        ImGui::End();
    }
    //creates the inspector window and sets up its logic
    void MakeInspector() {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 1));
        ImGui::Begin("Object Inspector");
        ImGui::PopStyleColor();
        if (selectedObject != nullptr) {
            //display data if we are playing, but if not, data is editable
            if (playing) {
                Inspector::BuildPlayModeInspector(selectedObject);
            } else {
                //std::cout << std::to_string(selectedObject->layer) << std::endl;
                Inspector::BuildNormalInspector(selectedObject, &layeredObjectsToLoad[0], currRenderer);
                //std::cout << std::to_string(selectedObject->layer) << std::endl;
                if (ImGui::Button("Delete")) {
                    std::cout << std::to_string(selectedObject->layer) << std::endl;
                    for (size_t i = 0; i < layeredObjectsToLoad[selectedObject->layer].size(); i++) {
                        if (layeredObjectsToLoad[selectedObject->layer][i] == selectedObject) {
                            SDL_Log("Deleted");
                            //std::cout << std::to_string(layeredObjectsToLoad[selectedObject->layer].size()) << std::endl;
                            layeredObjectsToLoad[selectedObject->layer].erase(layeredObjectsToLoad[selectedObject->layer].begin() + i);
                            //std::cout << std::to_string(layeredObjectsToLoad[selectedObject->layer].size()) << std::endl;
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
            std::string savePath = CreateSaveDialogBox();
            if (!savePath.empty()) {
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
            selectedObject = nullptr;
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
                for (Uint8 i = 0; i < layeredObjectsToLoad.size(); i++) {
                    for (GameObject* &gObj : layeredObjectsToLoad[i]) {
                        layeredObjectsSaved[i].push_back(gObj->Clone(currRenderer));
                        gObj->CreateAndPlaceBody(phyWorld);
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
                for (Uint8 i = 0; i < layeredObjectsToLoad.size(); i++) {
                    //destroy our game world
                    for (GameObject* &gObj : layeredObjectsToLoad[i]) {
                        if (gObj->objBody != nullptr) phyWorld->DestroyBody(gObj->objBody);
                        delete gObj; //This delete's the gomeObjects
                    }
                    layeredObjectsToLoad[i].clear(); //This delete's the pointers

                    //rebuild the world using saved objects
                    for (GameObject* &gObj : layeredObjectsSaved[i]) {
                        layeredObjectsToLoad[i].push_back(gObj->Clone(currRenderer));
                        delete gObj;
                    }
                    layeredObjectsSaved[i].clear();
                }
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
        for (std::vector<GameObject*> &layer : layeredObjectsToLoad) {
            for (GameObject* &gObj : layer) {
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
        return nullptr;
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
                newObj->SetUpdateFunction(DefaultUpdates::MovePlayer);
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
                (gObj->updateFunction)(gObj);
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

    //These are all the functions that handle saving and loading files in/out of the engine
    bool LoadFile(std::string path) {
        //first, load the file
        std::ifstream srcFile(path);
        std::string input;
        //then delete the current game
        for (std::vector<GameObject*> &layer : layeredObjectsToLoad) {
            for (GameObject* &gObj : layer) {
                delete gObj;
            }
            layer.clear();
        }
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
            GameObject* newObj = nullptr;
            b2BodyDef newBody;
            newBody.position.Set(std::stof(gObjPieces[0]), std::stof(gObjPieces[1]));
            newBody.angle = std::stof(gObjPieces[2]);
            newBody.type = (b2BodyType) std::stoi(gObjPieces[3]);
            newObj = new GameObject(currRenderer, newBody, (GameObject::Shape) std::stoi(gObjPieces[4]), std::stof(gObjPieces[9]), std::stof(gObjPieces[10]), input, std::stof(gObjPieces[11]), std::stof(gObjPieces[12]));
            newObj->SetUpdateFunction(DefaultUpdates::GetFunctionFromNumber(std::stoi(gObjPieces[14])));
            newObj->layer = std::stoi(gObjPieces[13]);
            newObj->color = SDL_Color(std::stoi(gObjPieces[5]), std::stoi(gObjPieces[6]), std::stoi(gObjPieces[7]), std::stoi(gObjPieces[8]));
            AddToViewPort(newObj);
        }
        srcFile.close();
        return true;
    }
    bool SaveFile(std::string path) {
        //write all the gameObjects to the file, each terminated by new line (\n)
        std::ofstream dstFile(path, std::ofstream::trunc);
        std::string newLine = "";
        for (std::vector<GameObject*> &layer : layeredObjectsToLoad) {
            for (GameObject* &gObj : layer) {
                newLine += std::to_string(gObj->objBodyDef.position.x) + ":";
                newLine += std::to_string(gObj->objBodyDef.position.y) + ":";
                newLine += std::to_string(gObj->objBodyDef.angle) + ":";
                newLine += std::to_string(gObj->objBodyDef.type) + ":";
                newLine += std::to_string(gObj->objShape) + ":";
                newLine += std::to_string(gObj->color.r) + ":";
                newLine += std::to_string(gObj->color.g) + ":";
                newLine += std::to_string(gObj->color.b) + ":";
                newLine += std::to_string(gObj->color.a) + ":";
                newLine += std::to_string(gObj->width) + ":";
                newLine += std::to_string(gObj->height) + ":";
                newLine += std::to_string(gObj->density) + ":";
                newLine += std::to_string(gObj->friction) + ":";
                newLine += std::to_string(gObj->layer) + ":";
                newLine += std::to_string(DefaultUpdates::GetFunctionNumber(gObj->updateFunction)) + ":";
                newLine += gObj->GetFilePath();
                dstFile << newLine << std::endl;
                newLine = "";
            }
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
            size_t origsize = wcslen(ppszName) + 1;
            size_t convertedChars = 0;
            const size_t newsize = origsize * 2;
            char* nstring = new char[newsize];
            wcstombs_s(&convertedChars, nstring, newsize, ppszName, _TRUNCATE);
            std::string filePath(nstring);
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
            size_t origsize = wcslen(ppszName) + 1;
            size_t convertedChars = 0;
            const size_t newsize = origsize * 2;
            char* nstring = new char[newsize];
            wcstombs_s(&convertedChars, nstring, newsize, ppszName, _TRUNCATE);
            std::string filePath(nstring);
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