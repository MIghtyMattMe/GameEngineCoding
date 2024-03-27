#include "GameObject.h"
#include "imgui/imgui.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_image.h"
#include "box2d/box2d.h"
#include <iostream>

GameObject::GameObject(SDL_Renderer* renderer, b2BodyDef targetBody, int shape, float targetWidth, float targetHeight, std::string textureFile) {
    objBodyDef = targetBody;
    width = targetWidth;
    height = targetHeight;
    objShape = (Shape) shape;
    SetTextureFromeFile(renderer, textureFile);
}
GameObject::~GameObject() {
    if (targetTexture != nullptr || targetTexture != NULL) SDL_DestroyTexture(targetTexture);
    targetTexture = NULL;
    objBody = NULL;
}

void GameObject::SetTextureFromeFile(SDL_Renderer* renderer, std::string textureFile) {
    textureFilePath = textureFile;
    targetTexture = IMG_LoadTexture(renderer, &textureFile[0]);
}

void GameObject::Update() {
    //nothing yet
}