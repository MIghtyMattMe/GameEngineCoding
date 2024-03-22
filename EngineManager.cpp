#include "EngineManager.h"
#include "Brush.h"
#include "imgui/imgui.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_image.h"
#include <iostream>

namespace EngineManager {
    Brush* brush = nullptr;
    std::vector<GameObject*> objectsToLoad = std::vector<GameObject*>();
    GameObject* selectedObject = nullptr;

    //Initialized the engine by creating out brush (and other stuff eventually)
    void InitEngine() {
        brush = new Brush();
    }

    //remove all our allocated memory
    void CloseEngine() {
        delete brush;
        brush = NULL;
        for (GameObject* gObj : objectsToLoad) {
            delete gObj;
            gObj = NULL;
        }
        //selectedObj is still in list of objectsToLoad, so it already
        //was deleted, but we still need to nullify the pointer
        selectedObject = NULL;
    }

    //Main Render Loop for our engine interface
    void RenderEngine() {
        MakeTools();
        MakeInspector();
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
        if (selectedObject) {
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
    void SelectObject(GameObject* clickedObj) {
        selectedObject = clickedObj;
    }
    GameObject* FindSelectedObject(SDL_Renderer* renderer, ImVec2 clickedPos) {
        for (GameObject* gObj : objectsToLoad) {
            //find if the clicked position is on the object's rectangle
            float leftLimit = gObj->position.x - (gObj->width / 2);
            float rightLimit = gObj->position.x + (gObj->width / 2);
            float upLimit = gObj->position.y - (gObj->height / 2);
            float downLimit = gObj->position.y + (gObj->height / 2);
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
    void AddToViewPort(SDL_Renderer* renderer, ImVec2 targetPos, float targetWidth, float targetHeight, std::string textureFile) {
        GameObject* newObj = nullptr;
        if (textureFile.empty()) {
            if (brush->getTextureFile().empty()) {
                SDL_Log("No brush texture was chosen!");
                return;
            }
            newObj = new GameObject(renderer, targetPos, targetWidth, targetHeight, brush->getTextureFile());
        } else {
            newObj = new GameObject(renderer, targetPos, targetWidth, targetHeight, textureFile);
        }
        objectsToLoad.push_back(newObj);
    }

    //handles the actual drawing of textures
    void DrawViewPort(SDL_Renderer* renderer) {
        //go over every object, make a rectangle, and apply the texture
        for (GameObject* gObj : objectsToLoad) {
            SDL_FRect texture_rect;
            texture_rect.x = gObj->position.x - (gObj->width / 2); //subtracting half width/height makes the image centered on the mouse
            texture_rect.y = gObj->position.y - (gObj->height / 2);
            texture_rect.w = gObj->width;
            texture_rect.h = gObj->height;
            SDL_RenderTexture(renderer, gObj->targetTexture, NULL, &texture_rect);
        }
    }
}