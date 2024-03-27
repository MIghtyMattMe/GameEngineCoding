#pragma once

#include "imgui/imgui.h"
#include "SDL3/SDL.h"
#include <string>

class Brush {
public:
    Brush();
    ~Brush();
    enum Type {None = 0, Circle = 1, Square = 2, Triangle = 3};
    void setType(Type brushType);
    std::string getTextureFile() { return textureToLoad; }
    Type getType() {return currType; }
private:
    Type currType = None;
    std::string textureToLoad = "";
};