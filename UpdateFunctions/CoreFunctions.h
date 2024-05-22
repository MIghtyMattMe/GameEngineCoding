#pragma once
#include "../GameObject.h"
#include "box2d/box2d.h"
#include "KeyData.h"
#include <iostream>

namespace CoreUpdateFunctions {
    extern bool alwaysTrue = true;
    extern bool key = false;
    extern bool touch = false;
    extern GameObject touchObj;
    void KeyPressed(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        int keyToCheck = ((int) data.x & 0xFF) - 93;
        key = KeyData::KeyDown((SDL_Scancode) keyToCheck);
    }
    void KeyJustPressed(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        int keyToCheck = ((int) data.x & 0xFF) - 93;
        key = KeyData::KeyJustPressed((SDL_Scancode) keyToCheck);
    }
    void KeyJustReleased(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        int keyToCheck = ((int) data.x & 0xFF) - 93;
        key = KeyData::KeyJustReleased((SDL_Scancode) keyToCheck);
    }
    void Push(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        myObj->objBody->ApplyLinearImpulseToCenter(data, true);
    }
    void Velocity(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        myObj->objBody->SetLinearVelocity(data);
    }
};