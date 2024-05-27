#include "Brush.h"

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
            textureToLoad = "resources/Square.png";
            break;
        case 3:
            textureToLoad = "resources/Square.png";
            break;
        case 4:
            textureToLoad = "resources/Square.png";
            break;
        case 5:
            textureToLoad = "resources/Player.png";
            break;
    }
}