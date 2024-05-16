#pragma once
#include "EngineManager.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <ShObjIdl_core.h>

namespace SaveLoadBuild {
    bool CopyMyFile(std::string srcPath, std::string dstPath) {
        std::filesystem::path source = srcPath;
        std::filesystem::path destination = dstPath;
        auto target = destination / source.filename();
        try {
            std::filesystem::create_directories(destination);
            std::filesystem::copy_file(source, target, std::filesystem::copy_options::overwrite_existing);
        } catch (std::exception& e) {
            std::cout << e.what();
            return false;
        }
        return true;
    }

    //These are all the functions that handle saving and loading files in/out of the engine
    bool LoadFile(std::string path, SDL_Renderer* &currRenderer, std::vector<std::vector<GameObject*>> &layeredObjectsToLoad) {
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
            //this assumes that the last item of the saved gameObj is always the texture path
            GameObject* newObj = nullptr;
            b2BodyDef newBody;
            newBody.position.Set(std::stof(gObjPieces[0]), std::stof(gObjPieces[1]));
            newBody.angle = std::stof(gObjPieces[2]);
            newBody.type = (b2BodyType) std::stoi(gObjPieces[3]);
            newObj = new GameObject(currRenderer, newBody, (GameObject::Shape) std::stoi(gObjPieces[4]), std::stof(gObjPieces[9]), std::stof(gObjPieces[10]), input, std::stof(gObjPieces[11]), std::stof(gObjPieces[12]));
            newObj->UpdateFunction = gObjPieces[14];
            newObj->layer = std::stoi(gObjPieces[13]);
            newObj->color = SDL_Color(std::stoi(gObjPieces[5]), std::stoi(gObjPieces[6]), std::stoi(gObjPieces[7]), std::stoi(gObjPieces[8]));

            //set up verts
            if (newObj->objShape == GameObject::Polygon) {
                newObj->verts.clear();
                std::string vertInput;
                std::getline(srcFile, vertInput);
                size_t vertLineIndex = 0;
                std::string gObjVertPiece = "";
                std::vector<std::string> gObjVertPieces = std::vector<std::string>();
                while ((vertLineIndex = vertInput.find(':')) != std::string::npos) {
                    gObjVertPiece = vertInput.substr(0, vertLineIndex);
                    gObjVertPieces.push_back(gObjVertPiece);
                    vertInput.erase(0, vertLineIndex + 1);
                }
                for (size_t i = 0; i < gObjVertPieces.size(); i++) {
                    b2Vec2 newVert = b2Vec2();
                    newVert.x = std::stof(gObjVertPieces.at(i));
                    i++;
                    newVert.y = std::stof(gObjVertPieces.at(i));
                    newObj->verts.push_back(newVert);
                }
            }
            EngineManager::AddToViewPort(newObj);
        }
        srcFile.close();
        return true;
    }

    bool SaveFile(std::string path, SDL_Renderer* &currRenderer, std::vector<std::vector<GameObject*>> &layeredObjectsToLoad) {
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
                newLine += (gObj->UpdateFunction) + ":";
                //newLine += std::to_string(DefaultUpdates::GetFunctionNumber(gObj->updateFunction)) + ":";
                newLine += gObj->GetFilePath();
                dstFile << newLine << std::endl;
                newLine = "";
                if (gObj->objShape == GameObject::Polygon) {
                    for (b2Vec2 vertex : gObj->verts) {
                        newLine += std::to_string(vertex.x) + ":";
                        newLine += std::to_string(vertex.y) + ":";
                    }
                    dstFile << newLine << std::endl;
                    newLine = "";
                }
            }
        }
        dstFile.close();
        return true;
    }

    bool BuildGame(std::string buildPath, std::string buildName, SDL_Renderer* &currRenderer, std::vector<std::vector<GameObject*>> &layeredObjectsToLoad) {
        if (!CopyMyFile("SDL3.dll", buildPath)) return false;
        if (!CopyMyFile("SDL3_image.dll", buildPath)) return false;
        std::filesystem::remove_all(buildPath + "/resources");
        std::filesystem::create_directories(buildPath + "/resources");
        std::filesystem::create_directories(buildPath + "/build");
        std::filesystem::copy("resources/", buildPath + "/resources/");
        SaveFile(buildPath + "/main.smol", currRenderer, layeredObjectsToLoad);
        std::string cmakeCMD = "cmake --build " + buildPath + "/build --config Debug";
        //std::string pathCMD = "cd " + buildPath;
        std::string configCMD = "cmake -A Win32 -B " + buildPath + "/build -S ./BuildCode";
        //system(pathCMD.c_str());
        system(configCMD.c_str());
        system(cmakeCMD.c_str());
        std::filesystem::remove(buildPath + "/" + buildName);
        std::filesystem::copy_file(buildPath + "/build/Debug/Engine1.exe", buildPath + "/" + buildName);
        std::filesystem::remove_all(buildPath + "/build");
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
    std::string GetFolderDialogBox() {
        COMDLG_FILTERSPEC ComDlgFS[1] = {{L"Executable (*.exe)", L"*.exe"}};
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
            if (filePath.substr(filePath.length() - 5) != ".exe") filePath += ".exe";
            //std::ofstream dstFile(filePath);
            finalPath = filePath;
            //dstFile.close();
            pShellItem->Release();
        }
        pFileSave->Release();
        CoUninitialize();
        return finalPath;
    }
};