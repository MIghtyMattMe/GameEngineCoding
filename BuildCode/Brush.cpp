#include "Brush.h"

Brush::Brush(SDL_Renderer* currRenderer) {
    texturesSaved[0] = NULL;
    texturesSaved[1] = IMG_LoadTexture(currRenderer, "resources/Circle.png");
    texturesSaved[2] = IMG_LoadTexture(currRenderer, "resources/Square.png");
    texturesSaved[3] = IMG_LoadTexture(currRenderer, "resources/Square.png");
    texturesSaved[4] = IMG_LoadTexture(currRenderer, "resources/Square.png");
    texturesSaved[5] = IMG_LoadTexture(currRenderer, "resources/Player.png");
}
Brush::~Brush() {
    SDL_DestroyTexture(texturesSaved[1]);
    SDL_DestroyTexture(texturesSaved[2]);
    SDL_DestroyTexture(texturesSaved[3]);
    SDL_DestroyTexture(texturesSaved[4]);
    SDL_DestroyTexture(texturesSaved[5]);
    texturesSaved[1] = NULL;
    texturesSaved[2] = NULL;
    texturesSaved[3] = NULL;
    texturesSaved[4] = NULL;
    texturesSaved[5] = NULL;
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