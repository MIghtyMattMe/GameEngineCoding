#include "GameObject.h"
#include "imgui/imgui.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_image.h"
#include <iostream>

GameObject::GameObject(SDL_Renderer* renderer, ImVec2 targetPos, float targetWidth, float targetHeight, std::string textureFile) {
    position = targetPos;
    width = targetWidth;
    height = targetHeight;
    SetTextureFromeFile(renderer, textureFile);
}
GameObject::~GameObject() {
    if (targetTexture != nullptr || targetTexture != NULL) SDL_DestroyTexture(targetTexture);
    targetTexture = NULL;
}

void GameObject::SetTextureFromeFile(SDL_Renderer* renderer, std::string textureFile) {
    textureFilePath = textureFile;
    targetTexture = IMG_LoadTexture(renderer, &textureFile[0]);
}

void GameObject::Update() {
    //nothing yet
}