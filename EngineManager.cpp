#include "EngineManager.h"
#include "Brush.h"
#include "imgui/imgui.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_image.h"
#include <iostream>

namespace EngineManager {
    Brush* brush = nullptr;
    std::vector<GameObject*> objectsToLoad = std::vector<GameObject*>();

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
        ImGui::Begin("GameRender");
        ImGui::PopStyleColor();
        ImGui::Text("Current Game State");
        ImGui::End();
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
            texture_rect.x = gObj->position.x - ((int)gObj->width / 2); //subtracting half width/height makes the image centered on the mouse
            texture_rect.y = gObj->position.y - ((int)gObj->height / 2);
            texture_rect.w = gObj->width;
            texture_rect.h = gObj->height;
            SDL_RenderTexture(renderer, gObj->targetTexture, NULL, &texture_rect);
        }
    }
}