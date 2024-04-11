#include "SDL3/SDL.h"
#include "box2d/box2d.h"

#include "EngineManager.h"
#include <iostream>
#include <string>

// Main code
int main(int argc, char* argv[])
{
    //init SDL windows and rendering system
    SDL_Init(SDL_INIT_CAMERA | SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMEPAD);
    SDL_Window* mainWindow = SDL_CreateWindow(
        "mainWindow",
        1280,
        720,
        0//SDL_WINDOW_RESIZABLE
    );
    SDL_Renderer* mainRenderer = SDL_CreateRenderer(mainWindow, nullptr, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    //init Engine
    EngineManager::InitEngine(mainRenderer);
    //create and set the physics world for the engine manager
    b2World phyWorld(EngineManager::GetGravityVector());
    EngineManager::SetPhyWorld(&phyWorld);
    EngineManager::LoadFile("./main.smol");

    bool done = false;
    bool mouseMiddleDown = false; //This is for knowing when to move the camera

    //This is our main rendering loop that gets called "every frame"
    while (!done)
    {

        EngineManager::UpdateGameObjects();
        KeyData::playModeInput.clear();
        KeyData::lastFrameKeys = KeyData::keysPressed;

        //set background color of renderer and fill it
        SDL_SetRenderDrawColor(mainRenderer, 20, 20, 20, 255);
        SDL_RenderClear(mainRenderer);
        EngineManager::StartPlay();

        //Draw our gameObjects and ImGui Overlay from the renderstack
        EngineManager::DrawViewPort();

        // Poll and handle messages (inputs, window resize, etc.)
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            //handles closing the tab
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
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
    }

    //shutdown and close all our things
    EngineManager::CloseEngine();
    SDL_DestroyRenderer(mainRenderer);
    SDL_DestroyWindow(mainWindow);
    SDL_Quit();

    return 0;
}
