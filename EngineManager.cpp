#include "EngineManager.h"
#include "Brush.h"
#include "imgui/imgui.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_image.h"
#include <iostream>

namespace EngineManager {
    Brush* brush = nullptr;
    std::vector<GameObject*> objectsToLoad = std::vector<GameObject*>();
    void InitEngine() {
        brush = new Brush();
    }

    void RenderEngine() {
        MakeTools();
        MakeViewPort();
    }

    void MakeTools() {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 1));
        ImGui::Begin("EngineTools");
        ImGui::PopStyleColor();
        if (ImGui::Button("Circle")) {
            brush->setType(Brush::Type::Circle);
        }
        if (ImGui::Button("Square")) {
            brush->setType(Brush::Type::Square);
        }
        if (ImGui::Button("Triangle")) {
            brush->setType(Brush::Type::Triangle);
        }
        ImGui::End();
    }

    void MakeViewPort() {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 1));
        ImGui::Begin("GameRender");
        ImGui::PopStyleColor();
        ImGui::Text("Current Game State");
        ImGui::End();
    }

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
    void DrawViewPort(SDL_Renderer* renderer) {
        for (GameObject* gObj : objectsToLoad) {
            SDL_FRect texture_rect;
            texture_rect.x = gObj->position.x - ((int)gObj->width / 2); //the x coordinate
            texture_rect.y = gObj->position.y - ((int)gObj->height / 2); //the y coordinate
            texture_rect.w = gObj->width; //the width of the texture
            texture_rect.h = gObj->height; //the height of the texture
            SDL_RenderTexture(renderer, gObj->targetTexture, NULL, &texture_rect);
        }
    }
}