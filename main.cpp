// Dear ImGui: standalone example application for Win32 + OpenGL 3

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

// This is provided for completeness, however it is strongly recommended you use OpenGL with SDL or GLFW.

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
        SDL_WINDOW_RESIZABLE
    );
    SDL_Renderer* mainRenderer = SDL_CreateRenderer(mainWindow, nullptr, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    //init ImGui and connect it to SDL
    ImGui::CreateContext();
    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(0, 0, 0, 0);
    ImGui_ImplSDL3_InitForSDLRenderer(mainWindow, mainRenderer);
    ImGui_ImplSDLRenderer3_Init(mainRenderer);

    //init Engine
    EngineManager::InitEngine();

    // Main loop
    bool done = false;
    bool mouseProcessingEvent = false;
    ImGuiIO& imguiIO = ImGui::GetIO();
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            } else if (imguiIO.WantCaptureMouse) {
                //don't handle the this input to SDL/Renderer
            } else if (!mouseProcessingEvent && event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
                mouseProcessingEvent = true;
                EngineManager::AddToViewPort(mainRenderer, ImVec2(event.button.x, event.button.y), 50.0f, 50.0f);
            } else if (mouseProcessingEvent && event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT) {
                mouseProcessingEvent = false;
            }
            ImGui_ImplSDL3_ProcessEvent(&event);
        }
        if (done) {
            break;
        }

        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        //Render Engine
        EngineManager::RenderEngine();

        ImGui::Render();

        //set background color of renderer
        SDL_SetRenderDrawColor(mainRenderer, 0, 0, 0, 0);
        //fills renderer with background color
        SDL_RenderClear(mainRenderer);

        EngineManager::DrawViewPort(mainRenderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData());

        //update renderer with any new render calls since last "RenderPreset" call
        SDL_RenderPresent(mainRenderer);
    }

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(mainRenderer);
    SDL_DestroyWindow(mainWindow);
    SDL_Quit();

    return 0;
}
