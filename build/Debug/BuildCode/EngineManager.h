#pragma once
#include "imgui/imgui.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_image.h"
#include "box2d/box2d.h"
#include "GameObject.h"
#include "UpdateFunctions/Default.h"
#include <vector>

//these are used for debugging and saving/loading files
#include <iostream>
#include <fstream>
#include <filesystem>
#include <ShObjIdl_core.h>

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
    void AddToViewPort(b2Vec2 targetPos, float targetWidth, float targetHeight, std::string textureFile="");
    //at some point switch to using Render_Geometry? (https://wiki.libsdl.org/SDL3/SDL_RenderGeometry) for custom polygons
    void DrawViewPort();

    //functions for selecting items
    void SelectObject(GameObject* clickedObj);
    GameObject* FindSelectedObject(b2Vec2 clickedPos);

    //Runs the Update Loop on gameObjects when playing
    void UpdateGameObjects();
    void ReadPlayInput(SDL_Event event);

    //Saves and Loads the project from a file
    bool LoadFile(std::string path);
    bool SaveFile(std::string path);
    std::string CreateLoadDialogBox();
    std::string CreateSaveDialogBox();

    //building projects
    std::string GetFolderDialogBox();
    bool BuildGame(std::string buildPath);
    bool CopyMyFile(std::string srcPath, std::string dstPath);

    //Controlling the Camera offset
    b2Vec2 GetCameraPosition();
    void SetCameraPosition(b2Vec2 newPos);

    //these are used for physics world creation in main.cpp
    b2Vec2 GetGravityVector();
    void SetPhyWorld(b2World* world);

    //Used to convert the pixel space of the viewport to meter space of the physics engine
    float MeterToPixel(float meters);
    float PixToMeter(float pixels);
    b2Vec2 VectorMeterToPixel(b2Vec2 meterVec);
    b2Vec2 VectorPixelToMeter(b2Vec2 pixelVec);
};
