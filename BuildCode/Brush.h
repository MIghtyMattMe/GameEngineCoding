#pragma once
#include "SDL3/SDL.h"
#include "SDL3/SDL_image.h"
#include <string>

class Brush {
public:
    Brush(SDL_Renderer* currRenderer);
    ~Brush();
    enum Type {None = 0, Circle = 1, Square = 2, Triangle = 3, Custom = 4, Player = 5};
    void setType(Type brushType);
    std::string getTextureFile() { return textureToLoad; }
    Type getType() {return currType; }
    SDL_Texture *texturesSaved[6] = {};
private:
    Type currType = None;
    std::string textureToLoad = "";
};