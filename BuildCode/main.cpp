#include "SDL3/SDL.h"
#include "box2d/box2d.h"

#include "EngineManager.h"
#include "UpdateFunctions/KeyData.h"
#include <iostream>
#include <string>

SDL_Window* mainWindow = nullptr;
SDL_Renderer* mainRenderer = nullptr;
b2World* phyWorld = nullptr;

bool SetUp() {
    //init SDL windows and rendering system
    if (SDL_Init(SDL_INIT_CAMERA | SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMEPAD)) return false;
    mainWindow = SDL_CreateWindow(
        "mainWindow",
        1280,
        720,
        0//SDL_WINDOW_RESIZABLE
    );
    if (!mainWindow) return false;
    mainRenderer = SDL_CreateRenderer(mainWindow, nullptr, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (!mainRenderer) return false;


    //init Engine
    EngineManager::InitEngine(mainRenderer);
    //create and set the physics world for the engine manager
    phyWorld = new b2World(EngineManager::GetGravityVector());
    EngineManager::SetPhyWorld(phyWorld);
    return true;
}

void TearDown() {
    //shutdown and close all our things
    EngineManager::CloseEngine();
    SDL_DestroyRenderer(mainRenderer);
    SDL_DestroyWindow(mainWindow);
    phyWorld = nullptr;
    SDL_Quit();
}

bool RenderLoop() {
    EngineManager::UpdateGameObjects();
    KeyData::playModeInput.clear();
    KeyData::lastFrameKeys = KeyData::keysPressed;

    //set background color of renderer and fill it
    if (SDL_SetRenderDrawColor(mainRenderer, 20, 20, 20, 255)) return false;
    if (SDL_RenderClear(mainRenderer)) return false;

    //Draw our gameObjects and ImGui Overlay from the renderStack
    EngineManager::DrawViewPort();
    return true;
}

// Main code
int main(int argc, char* argv[])
{
    if (!SetUp()) {
        std::cout << "Engine Failed SetUp" << std::endl;
        return -1;
    }

    bool done = false;
    bool mouseMiddleDown = false; //This is for knowing when to move the camera
    bool start = false;

    //This is our main rendering loop that gets called "every frame"
    while (!done)
    {

        if (!RenderLoop()) {
            std::cout << "Render Loop Error" << std::endl;
            return -1;
        }

        // Poll and handle messages (inputs, window resize, etc.)
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            //handles closing the tab
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
            
            //handles paint objects with the brush
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
                b2Vec2 posInMeters = EngineManager::VectorPixelToMeter(b2Vec2(event.button.x, event.button.y));
                EngineManager::AddToViewPort(posInMeters, 1.0f, 1.0f);
            }

            //handles moving the camera and zooming the camera
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_MIDDLE) {
                mouseMiddleDown = true;
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_MIDDLE) {
                mouseMiddleDown = false;
            } else if (mouseMiddleDown && event.type == SDL_EVENT_MOUSE_MOTION) {
                b2Vec2 newCamPos = EngineManager::GetCameraPosition();
                newCamPos.x -= EngineManager::PixToMeter(event.motion.xrel);
                newCamPos.y -= EngineManager::PixToMeter(event.motion.yrel);
                EngineManager::SetCameraPosition(newCamPos);
            }

            //record input for gameobjects and give it to the Engine
            if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
                EngineManager::ReadPlayInput(event);
            }
        }
        if (done) {
            break;
        }

        //update renderer with any new render calls since last "RenderPreset" call
        SDL_RenderPresent(mainRenderer);

        if (!start) {
            EngineManager::PlayEngine();
            start = true;
        }
    }

    TearDown();

    return 0;
}
