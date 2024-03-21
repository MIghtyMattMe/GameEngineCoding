#include "Brush.h"
#include "imgui/imgui.h"
#include "SDL3/SDL.h"

Brush::Brush() {
}
Brush::~Brush() {
}

void Brush::setType(Type brushType) {
    currType = brushType;
    switch (currType) {
        default:
        case 0:
            textureToLoad = "";
            break;
        case 1:
            textureToLoad = "resources/Circle.png";
            break;
        case 2:
            textureToLoad = "resources/Square.bmp";
            break;
        case 3:
            textureToLoad = "resources/Triangle.png";
            break;
    }
}