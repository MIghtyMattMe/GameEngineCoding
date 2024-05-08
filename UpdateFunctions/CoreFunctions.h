#pragma once
#include "../GameObject.h"
#include "box2d/box2d.h"
#include "KeyData.h"
#include <iostream>

namespace CoreUpdateFunctions {
    extern bool alwaysTrue = true;
    extern bool key;
    extern bool touch;
    extern GameObject touchObj;
    void KeyPressed(bool &conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        int keyToCheck = (int) data.x & 0xF + 3;
        key = KeyData::KeyDown((SDL_Scancode) keyToCheck);
    }
    void Push(bool &conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        myObj->objBody->ApplyLinearImpulseToCenter(data, true);
    }
};