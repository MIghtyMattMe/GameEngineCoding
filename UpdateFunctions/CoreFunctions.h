#pragma once
#include "../GameObject.h"
#include "box2d/box2d.h"
#include "KeyData.h"
#include <iostream>

namespace CoreUpdateFunctions {
    void KeyPressed(bool* conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        int key = (int) data.x & 0xF + 3;
        myObj->KeyPress = KeyData::KeyDown((SDL_Scancode) key);
    }
    void Push(bool* conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        myObj->objBody->ApplyLinearImpulseToCenter(data, true);
    }
};