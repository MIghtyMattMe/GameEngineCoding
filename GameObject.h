#pragma once
#include "SDL3/SDL.h"
#include "SDL3/SDL_image.h"
#include "box2d/box2d.h"
#include <vector>
#include <string>

class GameObject {
    public:
        GameObject();
        GameObject(SDL_Renderer* renderer, b2BodyDef targetBody, int shape=3, float targetWidth=1.0f, float targetHeight=1.0f, SDL_Texture *texturePtr=nullptr,  float targetDensity=1.0f, float targetFriction=0.3f);
        ~GameObject();

        //variables of a gameObject
        b2BodyDef objBodyDef;
        b2Body* objBody = nullptr;
        enum Shape {None = 0, Ellipse = 1, Box = 2, Polygon = 3};
        Shape objShape = None;
        SDL_Color color = SDL_Color(255, 255, 255, 255);
        std::vector<b2Vec2> verts = {b2Vec2(0, -0.5f), b2Vec2(0.5f, 0.5f), b2Vec2(-0.5f, 0.5f)};
        float width;
        float height;
        float density = 1.0f;
        float friction = 0.3f;
        int layer = 0;
        int tag = 0;

        std::string UpdateFunction = "";
        bool Grounded = false;

        //Getter for the target texture
        SDL_Texture* GetTexturePtr() { return targetTexture; }

        //operator overrides for checking this data structure
        bool operator==(GameObject other) {
            if (objBodyDef.position.x != other.objBodyDef.position.x) return false;
            if (objBodyDef.position.y != other.objBodyDef.position.y) return false;
            if (layer != other.layer) return false;
            if (tag != other.tag) return false;
            if (width != other.width) return false;
            if (height != other.height) return false;
            if (color.r != other.color.r) return false;
            if (color.g != other.color.g) return false;
            if (color.b != other.color.b) return false;
            if (color.a != other.color.a) return false;
            if (textureFilePath != other.textureFilePath) return false;
            if (objShape != other.objShape) return false;
            return true;
        }
        GameObject* Clone(SDL_Renderer* targetRenderer);
        void CreateAndPlaceBody(b2World* phyWorld);
    private:
        std::string textureFilePath;
        SDL_Texture* targetTexture;
};