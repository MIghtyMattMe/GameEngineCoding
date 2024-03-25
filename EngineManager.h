#pragma once
#include "imgui/imgui.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_image.h"
#include "box2d/box2d.h"
#include "GameObject.h"
#include <vector>
#include <string>

namespace EngineManager {

    //starting and ending the engine
    void InitEngine(SDL_Renderer* renderer);
    void CloseEngine();

    //creating the engine visual and logic
    void RenderEngine();
    void MakeTools();
    void MakeInspector();
    void MakeControlBoard();

    //functions for adding/drawing objects
    void AddToViewPort(GameObject* gameObject);
    void AddToViewPort(ImVec2 targetPos, float targetWidth, float targetHeight, std::string textureFile="");
    //at some point switch to using Render_Geometry? (https://wiki.libsdl.org/SDL3/SDL_RenderGeometry) for custom polygons
    void DrawViewPort();

    //functions for selecting items
    void SelectObject(GameObject* clickedObj);
    GameObject* FindSelectedObject(ImVec2 clickedPos);

    //Runs the Update Loop on gameObjects when playing
    void UpdateGameObjects();

    //Saves and Loads the project from a file
    bool LoadFile(std::string path);
    bool SaveFile(std::string path);
    std::string CreateLoadDialogBox();
    std::string CreateSaveDialogBox();

    //Controlling the Camera offset
    ImVec2 GetCameraPosition();
    void SetCameraPosition(ImVec2 newPos);

    //these are used for physics world creation in main.cpp
    b2Vec2 GetGravityVector();
    void SetPhyWorld(b2World* world);
};
