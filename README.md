# Smol Game Engine

This is a personal project by Matthew Meacham that uses [Dear IMGUI](https://github.com/ocornut/imgui), [SDL3](https://github.com/libsdl-org/SDL), and [Box2D](https://box2d.org/) to create a small 2D game engine. The purpose of this project is to experiment with building an application from the ground up. The Smol Game Engine is not meant for commercial use, however if anyone wants to utilize this they are free to do so.

## How To Build

Building the Smol Game Engine requires [CMake](https://cmake.org/) (version 3.5 or higher) and the Windows OS. Build the engine using CMake. You also need SDL3.dll and SDL3_image.dll in the same directory as the executable to run the engine.

## How To Use

Please refer to this page to see how to use the engine: [https://mightymattme.github.io/GameEngineCoding/](https://mightymattme.github.io/GameEngineCoding/)

## Contents Overview

- BuildCode/
  - This contains the code that the engine will run when building a finished game. Place in the same directory as the engine executable
- imgui/
  - This is the code for Dear IMGUI. It is used for all the UI elements in the engine.
- include/
  - Contains information needed for SDL3 and Box2D.
- lib/
  - Contains information needed for SDL3 and Box2D.
- UpdateFunctions/
  - This folder has code for the different update functions for objects in the game. It also houses data on reading player input.
- Brush.cpp
  - The brush class is what gets used for "painting" game objects in the engine. The main purpose of the brush is to keep track of image files that are being placed.
- EngineManager.cpp
  - Handles all the main processes of the game engine. It contains the code for drawing the Dear IMGUI elements, drawing the game objects, and editing the game state.
- GameObject.cpp
  - The core for all objects within the game. The positional data is handled with Box2D, and the update function is a function pointer to a function in UpdateFunctions/.
- Inspector.h
  - Contains the functions for the EngineManager that handles the inspector window. The inspector window is what is used to edit the variables of game objects.
- main.cpp
  - Contains the core loop of the engine.
- SaveBuildLoad.h
  - Contains the functions for Saving the game, loading the game, and building the game.
