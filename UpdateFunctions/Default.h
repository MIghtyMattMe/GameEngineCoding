#pragma once
#include "../GameObject.h"
#include "box2d/box2d.h"
#include "KeyData.h"
#include <iostream>

namespace DefaultUpdates {
    int GetFunctionNumber(void (*function)(void* gObj));
    void (*GetFunctionFromNumber(int num))(void*);
    void Empty(void* gObj);
    void MovePlayer(void* gObj);
    void WinGameCollider(void* collisionObj, void* playerObj);
    void LoseGameCollider(void* collisionObj, void* playerObj);
};