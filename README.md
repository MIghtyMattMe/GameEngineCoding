# Smol Game Engine

This is a personal project by Matthew Meacham that uses [Dear IMGUI](https://github.com/ocornut/imgui), [SDL3](https://github.com/libsdl-org/SDL), and [Box2D](https://box2d.org/) to create a small 2D game engine. The purpose of this project is to experiment with building an application from the ground up. The Smol Game Engine is not meant for commercial use, however if anyone wants to utilize this they are free to do so.

## How To Build

Building the Smol Game Engine requires [CMake](https://cmake.org/) (version 3.5 or higher) and the Windows OS. Build the engine using CMake. You also need SDL3.dll and SDL3_image.dll in the same directory as the executable to run the engine. Additionally, to build a game from the engine you will need the BuildCode folder to be placed in the executable's directory.

## How To Use

There are 3 windows within the Smol Game Engine application:

- Engine Tools
  - This window contains a list of different objects you can "paint" in the game world. Simply left click on a button to select an object, and then left click anywhere in the window to place your object.
- Object Inspector
  - This will let you edit and view the properties of your game object. Right click on an object to have it revealed in the inspector. (You can only edit an object's properties in this window while the game is not being played.)
- Control Board
  - Contains all the buttons for saving a game (saved as a .smol file), loading a game, playing and stopping a game, and building a game to an executable.

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
