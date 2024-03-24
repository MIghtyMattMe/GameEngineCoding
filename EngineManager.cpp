//Custom scritps
#include "EngineManager.h"
#include "Brush.h"
#include "GameObject.h"

//SDL and ImGui database
#include "imgui/imgui.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_image.h"

//used for getting files from the user
#include <ShObjIdl_core.h>

//these are used for debugging and saving/loading files
#include <iostream>
#include <fstream>
#include <string>

namespace EngineManager {
    Brush* brush = nullptr;
    std::vector<GameObject*> objectsToLoad = std::vector<GameObject*>();
    bool playing = false;
    std::vector<GameObject*> savedObjects = std::vector<GameObject*>();
    GameObject* selectedObject = nullptr;

    SDL_Renderer* currRenderer = nullptr;

    ImVec2 cameraPos = ImVec2(0, 0);

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
            ImGui::SliderFloat("X Pos", &selectedObject->position.x, 0, 1280);
            ImGui::SliderFloat("Y Pos", &selectedObject->position.y, 0, 720);
            ImGui::SliderFloat("Width", &selectedObject->width, 0, 1000);
            ImGui::SliderFloat("Height", &selectedObject->height, 0, 1000);
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
                    GameObject* newObj = new GameObject(currRenderer, gObj->position, gObj->width, gObj->height, gObj->GetFilePath());
                    newObj->SetTextureFromeFile(currRenderer, newObj->GetFilePath());
                    savedObjects.push_back(newObj);
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
                    delete gObj; //This delete's the gomeObjects
                }
                objectsToLoad.clear(); //This delete's the pointers
                for (GameObject* gObj : savedObjects) {
                    GameObject* newObj = new GameObject(currRenderer, gObj->position, gObj->width, gObj->height, gObj->GetFilePath());
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
    GameObject* FindSelectedObject(ImVec2 clickedPos) {
        for (GameObject* gObj : objectsToLoad) {
            //find if the clicked position is on the object's rectangle
            float leftLimit = gObj->position.x - (gObj->width / 2) + cameraPos.x;
            float rightLimit = gObj->position.x + (gObj->width / 2) + cameraPos.x;
            float upLimit = gObj->position.y - (gObj->height / 2) + cameraPos.y;
            float downLimit = gObj->position.y + (gObj->height / 2) + cameraPos.y;
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
    void AddToViewPort(ImVec2 targetPos, float targetWidth, float targetHeight, std::string textureFile) {
        GameObject* newObj = nullptr;
        targetPos.x = targetPos.x - cameraPos.x;
        targetPos.y = targetPos.y - cameraPos.y;
        if (textureFile.empty()) {
            if (brush->getTextureFile().empty()) {
                SDL_Log("No brush texture was chosen!");
                return;
            }
            newObj = new GameObject(currRenderer, targetPos, targetWidth, targetHeight, brush->getTextureFile());
        } else {
            newObj = new GameObject(currRenderer, targetPos, targetWidth, targetHeight, textureFile);
        }
        objectsToLoad.push_back(newObj);
    }
    //handles the actual drawing of textures
    void DrawViewPort() {
        //go over every object, make a rectangle, and apply the texture
        for (GameObject* gObj : objectsToLoad) {
            SDL_FRect texture_rect;
            texture_rect.x = gObj->position.x - (gObj->width / 2) + cameraPos.x; //subtracting half width/height makes the image centered on the mouse
            texture_rect.y = gObj->position.y - (gObj->height / 2) + cameraPos.y;
            texture_rect.w = gObj->width;
            texture_rect.h = gObj->height;
            SDL_RenderTexture(currRenderer, gObj->GetTexturePtr(), NULL, &texture_rect);
        }
    }

    //Runs the Update Loop on gameObjects when playing
    void UpdateGameObjects() {
        if (!playing) return;
        for (GameObject* gObj : objectsToLoad) {
            gObj->Update();
        }
    }

    //Controlling the Camera offset
    ImVec2 GetCameraPosition() {
        return cameraPos;
    }
    void SetCameraPosition(ImVec2 newPos) {
        cameraPos = newPos;
    }

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
            std::cout << input << std::endl;
            size_t lineIndex = 0;
            std::string gObjPiece = "";
            std::vector<std::string> gObjPieces = std::vector<std::string>();
            while ((lineIndex = input.find(':')) != std::string::npos) {
                gObjPiece = input.substr(0, input.find(':'));
                std::cout << gObjPiece << std::endl;
                gObjPieces.push_back(gObjPiece);
                input.erase(0, lineIndex + 1);
            }
            //this assumes that the last item of the saved gameobj is always the texture path
            AddToViewPort(ImVec2(std::stof(gObjPieces[0]), std::stof(gObjPieces[1])), std::stof(gObjPieces[6]), std::stof(gObjPieces[7]), input);
        }
        srcFile.close();
        return true;
    }
    bool SaveFile(std::string path) {
        //write all the gameObjects to the file, each terminated by new line (\n)
        std::ofstream dstFile(path, std::ofstream::trunc);
        std::string newLine = "";
        for (GameObject* gObj : objectsToLoad) {
            newLine += std::to_string(gObj->position.x) + ":";
            newLine += std::to_string(gObj->position.y) + ":";
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