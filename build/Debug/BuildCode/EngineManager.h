#pragma once
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
    void StartPlay();

    //functions for adding/drawing objects
    void AddToViewPort(GameObject* gameObject);
    void DrawViewPort();

    //Runs the Update Loop on gameObjects when playing
    void UpdateGameObjects();
    void ReadPlayInput(SDL_Event event);

    //Saves and Loads the project from a file
    bool LoadFile(std::string path);

    //Controlling the Camera offset
    b2Vec2 GetCameraPosition();

    //these are used for physics world creation in main.cpp
    b2Vec2 GetGravityVector();
    void SetPhyWorld(b2World* world);

    //Used to convert the pixel space of the viewport to meter space of the physics engine
    float MeterToPixel(float meters);
    float PixToMeter(float pixels);
    b2Vec2 VectorMeterToPixel(b2Vec2 meterVec);
    b2Vec2 VectorPixelToMeter(b2Vec2 pixelVec);
};
