#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_sdlrenderer3.h"
#define SDL_MAIN_HANDLED
#include "SDL3/SDL.h"
#include "imgui/imgui.h"

#include "EngineManager.h"

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

    bool done = false;
    bool mouseMiddleDown = false; //This is for knowing when to move the camera
    ImGuiIO& imguiIO = ImGui::GetIO(); //imguiIO handles the input and output signals for imGui

    //This is our main rendering loop that gets called "every frame"
    while (!done)
    {
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
                EngineManager::AddToViewPort(ImVec2(event.button.x, event.button.y), 50.0f, 50.0f);
            }
            
            //handles selecting an object
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_RIGHT) {
                EngineManager::SelectObject(EngineManager::FindSelectedObject(ImVec2(event.button.x, event.button.y)));
            }

            //handles moving the camera and zooming the camera
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_MIDDLE) {
                mouseMiddleDown = true;
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_MIDDLE) {
                mouseMiddleDown = false;
            } else if (mouseMiddleDown && event.type == SDL_EVENT_MOUSE_MOTION) {
                ImVec2 newCamPos = EngineManager::GetCameraPosition();
                newCamPos.x += event.motion.xrel;
                newCamPos.y += event.motion.yrel;
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
