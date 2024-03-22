#pragma once
#include "imgui/imgui.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_image.h"
#include <vector>
#include <string>

namespace EngineManager {
    class GameObject {
        public:
            GameObject(SDL_Renderer* renderer, ImVec2 targetPos, float targetWidth, float targetHeight, std::string textureFile) {
                position = targetPos;
                width = targetWidth;
                height = targetHeight;
                SetTextureFromeFile(renderer, textureFile);
            }
            ~GameObject() {
                if (targetTexture != NULL) SDL_DestroyTexture(targetTexture);
                targetTexture = NULL;
            }
            ImVec2 position;
            SDL_Color color;
            float width;
            float height;
            SDL_Texture* targetTexture;
            void SetTextureFromeFile(SDL_Renderer* renderer, std::string textureFile) {
                targetTexture = IMG_LoadTexture(renderer, &textureFile[0]);
            }
    };

    void InitEngine();
    void CloseEngine();
    void RenderEngine();
    void MakeTools();
    void MakeInspector();
    void AddToViewPort(GameObject* gameObject);
    void AddToViewPort(SDL_Renderer* renderer, ImVec2 targetPos, float targetWidth, float targetHeight, std::string textureFile="");
    //at some point switch to using Render_Geometry? (https://wiki.libsdl.org/SDL3/SDL_RenderGeometry) for custom polygons
    void DrawViewPort(SDL_Renderer* renderer);
};