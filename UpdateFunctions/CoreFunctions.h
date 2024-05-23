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

    /*
    Key input functions
    */

    //Sets the 'key' registar to true if the specified key is currently being held down.
    //Only works with lowercase letters.
    void KeyPressed(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        int keyToCheck = ((int) data.x & 0xFF) - 93;
        key = KeyData::KeyDown((SDL_Scancode) keyToCheck);
    }
    //Sets the 'key' registar to true the frame the specified key is pressed down.
    //Only works with lowercase letters.
    void KeyJustPressed(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        int keyToCheck = ((int) data.x & 0xFF) - 93;
        key = KeyData::KeyJustPressed((SDL_Scancode) keyToCheck);
    }
    //Sets the 'key' registar to true the frame the specified key is released.
    //Only works with lowercase letters.
    void KeyJustReleased(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        int keyToCheck = ((int) data.x & 0xFF) - 93;
        key = KeyData::KeyJustReleased((SDL_Scancode) keyToCheck);
    }

    /*
    Movement based functions
    */

    //Applies a force, in the world space x and y directions specified, to the rigidbody calling Push().
    void Push(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        myObj->objBody->ApplyLinearImpulseToCenter(data, true);
    }
    //Sets the velocity for a single frame, in the worldspace x and y directions specified, the the gameobject calling Velocity().
    //This will override and gravitational force.
    void Velocity(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        myObj->objBody->SetLinearVelocity(data);
    }
    //Applies a force, in the local space x and y directions specified, to the rigidbody calling Push().
    void PushLocal(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        float rot = myObj->objBody->GetAngle();
        b2Vec2 rotVector = b2Vec2(data.x * cos(rot) - data.y * sin(rot), data.x * sin(rot) + data.y * cos(rot));
        myObj->objBody->ApplyLinearImpulseToCenter(rotVector, true);
    }
    //Sets the velocity for a single frame, in the local space x and y directions specified, the the gameobject calling Velocity().
    //This will override and gravitational force.
    void VelocityLocal(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        float rot = myObj->objBody->GetAngle();
        b2Vec2 rotVector = b2Vec2(data.x * cos(rot) - data.y * sin(rot), data.x * sin(rot) + data.y * cos(rot));
        myObj->objBody->SetLinearVelocity(rotVector);
    }
};