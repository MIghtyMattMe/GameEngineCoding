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
            bool operator==(GameObject other) {
                if (position.x != other.position.x) return false;
                if (position.y != other.position.y) return false;
                if (width != other.width) return false;
                if (height != other.height) return false;
                if (color.r != other.color.r) return false;
                if (color.g != other.color.g) return false;
                if (color.b != other.color.b) return false;
                if (color.a != other.color.a) return false;
                if (targetTexture != other.targetTexture) return false;
                return true;
            }
    };

    //starting and ending the engine
    void InitEngine();
    void CloseEngine();

    //creating the engine visual and logic
    void RenderEngine();
    void MakeTools();
    void MakeInspector();

    //functions for adding/drawing objects
    void AddToViewPort(GameObject* gameObject);
    void AddToViewPort(SDL_Renderer* renderer, ImVec2 targetPos, float targetWidth, float targetHeight, std::string textureFile="");
    //at some point switch to using Render_Geometry? (https://wiki.libsdl.org/SDL3/SDL_RenderGeometry) for custom polygons
    void DrawViewPort(SDL_Renderer* renderer);

    //functions for selecting items
    void SelectObject(GameObject* clickedObj);
    GameObject* FindSelectedObject(SDL_Renderer* renderer, ImVec2 clickedPos);
};