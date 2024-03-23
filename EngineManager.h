#pragma once
#include "imgui/imgui.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_image.h"
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
    bool LoadFile();
    bool SaveFile();
};