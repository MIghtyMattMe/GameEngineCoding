#pragma once
#include "SDL3/SDL.h"
#include "SDL3/SDL_image.h"
#include "box2d/box2d.h"
#include <vector>
#include <iostream>
#include <string>

namespace KeyData {
    extern std::vector<SDL_Scancode> playModeInput;
    extern Uint64 lastFrameKeys;
    extern Uint64 keysPressed;

    bool inline KeyDown(SDL_Scancode key) {
        if (key >= 64) return false;
        return (keysPressed & (1ULL << key));
    }
    bool inline KeyJustPressed(SDL_Scancode key) {
        if (key >= 64) return false;
        bool pressed = false;
        for (SDL_Scancode code : playModeInput) {
            if (code == key) {
                pressed = true;
                break;
            }
        }
        pressed = pressed && !(lastFrameKeys & (1ULL << key));
        return pressed;
    }
    bool inline KeyJustReleased(SDL_Scancode key) {
        if (key >= 64) return false;
        bool released = false;
        for (SDL_Scancode code : playModeInput) {
            if (code == key) {
                released = true;
                break;
            }
        }
        released = released && !(keysPressed & (1ULL << key));
        return released;
    }
};