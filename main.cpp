#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_sdlrenderer3.h"
#define SDL_MAIN_HANDLED
#include "SDL3/SDL.h"
#include "imgui/imgui.h"
#include "box2d/box2d.h"

#include "EngineManager.h"
#include <iostream>
#include <string>
//#include <sysinfoapi.h>

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

    //init ImGui and connect it to SDL
    ImGui::CreateContext();
    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(0, 0, 0, 0);
    ImGui_ImplSDL3_InitForSDLRenderer(mainWindow, mainRenderer);
    ImGui_ImplSDLRenderer3_Init(mainRenderer);

    //init Engine
    EngineManager::InitEngine(mainRenderer);
    //create and set the physics world for the engine manager
    b2World phyWorld(EngineManager::GetGravityVector());
    EngineManager::SetPhyWorld(&phyWorld);

    bool done = false;
    bool mouseMiddleDown = false; //This is for knowing when to move the camera
    ImGuiIO& imguiIO = ImGui::GetIO(); //imguiIO handles the input and output signals for imGui
    //SYSTEMTIME sysTime;
    //GetSystemTime(&sysTime);
    //int totalTime = 0;
    //int currTime;
    //int lastTime = sysTime.wMilliseconds;
    //float timeStep = 1.0f / 60.0f;

    //This is our main rendering loop that gets called "every frame"
    while (!done)
    {
        //update physics world
        /*
        GetSystemTime(&sysTime);
        currTime = sysTime.wMilliseconds;
        totalTime += ((currTime - lastTime) > 0) ? (currTime - lastTime) : (currTime - lastTime + 999);
        lastTime = currTime;
        if (totalTime >= 20) {
            phyWorld.Step(timeStep, 6, 2);
            totalTime = 0;
        }
        */

        EngineManager::UpdateGameObjects();

        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        //Add our engine overlay and ImGui stuff to the render stack
        EngineManager::RenderEngine();
        ImGui::Render();

        //set background color of renderer and fill it
        SDL_SetRenderDrawColor(mainRenderer, 20, 20, 20, 255);
        SDL_RenderClear(mainRenderer);

        //Draw our gameObjects and ImGui Overlay from the renderstack
        EngineManager::DrawViewPort();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData());

        // Poll and handle messages (inputs, window resize, etc.)
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            //handles closing the tab
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            } else if (imguiIO.WantCaptureMouse) {
                //don't handle the this input to SDL/Renderer, so we do nothing
            }
            
            //handles paint objects with the brush
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
                b2Vec2 posInMeters = EngineManager::VectorPixelToMeter(b2Vec2(event.button.x, event.button.y));
                EngineManager::AddToViewPort(posInMeters, 1.0f, 1.0f);
            }
            
            //handles selecting an object
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_RIGHT) {
                b2Vec2 posInMeters = EngineManager::VectorPixelToMeter(b2Vec2(event.button.x, event.button.y));
                EngineManager::SelectObject(EngineManager::FindSelectedObject(posInMeters));
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

            ImGui_ImplSDL3_ProcessEvent(&event);
        }
        if (done) {
            break;
        }

        //update renderer with any new render calls since last "RenderPreset" call
        SDL_RenderPresent(mainRenderer);
    }

    //shutdown and close all our things
    EngineManager::CloseEngine();
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(mainRenderer);
    SDL_DestroyWindow(mainWindow);
    SDL_Quit();

    return 0;
}
