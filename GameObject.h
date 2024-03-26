#pragma once
#include "imgui/imgui.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_image.h"
#include "box2d/box2d.h"
#include <vector>
#include <string>

class GameObject {
    public:
        GameObject(SDL_Renderer* renderer, b2BodyDef targetBody, float targetWidth, float targetHeight, std::string textureFile);
        ~GameObject();

        //variables of a gameobject
        b2BodyDef objBodyDef;
        b2Body* objBody = nullptr;
        SDL_Color color;
        float width;
        float height;

        //sets the texture of a gameobject from a file location
        void SetTextureFromeFile(SDL_Renderer* renderer, std::string textureFile);

        //Getter for the file path string and target texture
        std::string GetFilePath() { return textureFilePath; }
        SDL_Texture* GetTexturePtr() { return targetTexture; }

        //Update will be called once per frame and can be overriden by children
        void virtual Update();

        //operator overrides for checking this data structure
        bool operator==(GameObject other) {
            if (objBodyDef.position.x != other.objBodyDef.position.x) return false;
            if (objBodyDef.position.y != other.objBodyDef.position.y) return false;
            if (width != other.width) return false;
            if (height != other.height) return false;
            if (color.r != other.color.r) return false;
            if (color.g != other.color.g) return false;
            if (color.b != other.color.b) return false;
            if (color.a != other.color.a) return false;
            if (targetTexture != other.targetTexture) return false;
            return true;
        }
    private:
        std::string textureFilePath;
        SDL_Texture* targetTexture;
};