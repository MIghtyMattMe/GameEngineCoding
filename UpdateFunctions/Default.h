#pragma once
#include "../GameObject.h"
#include "box2d/box2d.h"
#include "KeyData.h"
#include <iostream>

namespace DefaultUpdates {
    void Empty(void* gObj);
    void MovePlayer(void* gObj);
};