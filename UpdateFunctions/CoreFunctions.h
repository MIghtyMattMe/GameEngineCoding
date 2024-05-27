#pragma once
#include "../GameObject.h"
#include "../EngineManager.h"
#include "box2d/box2d.h"
#include "KeyData.h"
#include <iostream>
#include <ShObjIdl_core.h>

namespace CoreUpdateFunctions {
    extern bool alwaysTrue = true;
    extern bool key = false;
    extern bool touch = false;
    extern GameObject* touchObj = nullptr;
    extern bool grounded = false;

    class RayCastCallback : public b2RayCastCallback {
    public:
        RayCastCallback(b2Fixture* src) { 
            srcFixture = src;
            outHit = false; 
        }
        virtual float ReportFixture(b2Fixture* fix, const b2Vec2& pnt, const b2Vec2& norm, float frac) {
            outPoint = pnt;
            outFixture = fix;
            outNorm = norm;
            outFraction = frac;
            if (fix->GetBody() != srcFixture->GetBody()) {
                outHit = true;
                return 0;
            }
            return -1;
        }
        bool outHit;
        b2Fixture* outFixture;
        b2Fixture* srcFixture;
        b2Vec2 outPoint;
        b2Vec2 outNorm;
        float outFraction;
    };

    /*
    * List of functions that set registers.
    * This consists of 3 parts, first ones the set the key register.
    * Next, is GetTouch which will detect what is being touched, set the register and save the game object touched.
    * Finally is the grounded register that is set to true when the player has at least one vertex on the ground.
    */

    /*
    Key input functions
    */

    //Sets the 'key' register to true if the specified key is currently being held down.
    //Only works with lowercase letters.
    void KeyPressed(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        int keyToCheck = ((int) data.x & 0xFF) - 93;
        key = KeyData::KeyDown((SDL_Scancode) keyToCheck);
    }
    //Sets the 'key' register to true the frame the specified key is pressed down.
    //Only works with lowercase letters.
    void KeyJustPressed(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        int keyToCheck = ((int) data.x & 0xFF) - 93;
        key = KeyData::KeyJustPressed((SDL_Scancode) keyToCheck);
    }
    //Sets the 'key' register to true the frame the specified key is released.
    //Only works with lowercase letters.
    void KeyJustReleased(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        int keyToCheck = ((int) data.x & 0xFF) - 93;
        key = KeyData::KeyJustReleased((SDL_Scancode) keyToCheck);
    }

    /*
    Touch functions
    */
    //checks what is touching the gameobject, if there is a tag provided it will only check for that tag,
    //otherwise, if the provided tag is -1, it will check for any touching object.
    void GetTouch(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        touch = false;
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        for (b2ContactEdge* ce = myObj->objBody->GetContactList(); ce; ce = ce->next)
        {
            //if the contact's obect has the right tag, or if we are looking for any object, grab it and set the register
            b2Contact* c = ce->contact;
            GameObject* touchTestObj = EngineManager::FindObjectFromBody(*(c->GetFixtureB()->GetBody()));
            if (touchTestObj == myObj) {
                touchTestObj = EngineManager::FindObjectFromBody(*(c->GetFixtureA()->GetBody()));
            }
            if (touchTestObj->tag == (int) data.x || data.x < 0) {
                touch = true;
                touchObj = touchTestObj;
            }
        }
    }

    /*
    Grounded functions
    */
    //Sets the 'grounded' register to true if at least one vertex is on the ground.
    //Only works with dynamic bodies.
    void IsGrounded(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        grounded = false;
        GameObject* myObj = (GameObject*) gObj;
        if (myObj->objBody->GetType() != b2_dynamicBody) {
            SDL_Log("IsGrounded cannot be called on objects without a dynamic body!");
            return;
        }
        b2Vec2 posOffset = myObj->objBody->GetPosition();
        float rotOffset = myObj->objBody->GetAngle();
        //iterate over the fixtures, then look at the shape and cycle over the vertices
        for (b2Fixture* fixture = myObj->objBody->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
            RayCastCallback rayData = RayCastCallback(fixture);

            //this is for ellipses that use the chain shape, iterating over every vertex (with position and rotation offsets)
            if (fixture->GetType() == b2Shape::e_chain) {
                b2ChainShape* chainShape = (b2ChainShape*) fixture->GetShape();
                size_t numberOfVerts = (myObj->verts.empty()) ? 4 : myObj->verts.size();
                for (size_t i = 0; i < 32; i++) {
                    b2Vec2 vertStart = chainShape->m_vertices[i];
                    std::cout << chainShape->m_vertices[i].x << std::endl;
                    vertStart = b2Vec2(vertStart.x * cos(rotOffset) - vertStart.y * sin(rotOffset), vertStart.x * sin(rotOffset) + vertStart.y * cos(rotOffset));
                    vertStart += posOffset;
                    b2Vec2 vertEnd = b2Vec2(vertStart.x, vertStart.y + 0.07f);
                    myObj->objBody->GetWorld()->RayCast(&rayData, vertStart, vertEnd);
                    if (rayData.outHit) {
                        grounded = true;
                        return;
                    }
                }
            
            } else if (fixture->GetType() == b2Shape::e_circle) {
                b2CircleShape* circleShape = (b2CircleShape*) fixture->GetShape();
                size_t numberOfVerts = (myObj->verts.empty()) ? 4 : myObj->verts.size();
                b2Vec2 vertStart = circleShape->m_p;
                vertStart = b2Vec2(vertStart.x, vertStart.y + circleShape->m_radius);
                vertStart += posOffset;
                b2Vec2 vertEnd = b2Vec2(vertStart.x, vertStart.y + 0.05f);
                myObj->objBody->GetWorld()->RayCast(&rayData, vertStart, vertEnd);
                if (rayData.outHit) {
                    grounded = true;
                    return;
                }
            
            //this is for polygons, iterating over every vertex (with position and rotation offsets)
            } else if (fixture->GetType() == b2Shape::e_polygon) {
                b2PolygonShape* polyShape = (b2PolygonShape*) fixture->GetShape();
                size_t numberOfVerts = (myObj->verts.empty()) ? 4 : myObj->verts.size();
                for (size_t i = 0; i < numberOfVerts + 1; i++) {
                    b2Vec2 vertStart = polyShape->m_vertices[i];
                    vertStart = b2Vec2(vertStart.x * cos(rotOffset) - vertStart.y * sin(rotOffset), vertStart.x * sin(rotOffset) + vertStart.y * cos(rotOffset));
                    vertStart += posOffset;
                    b2Vec2 vertEnd = b2Vec2(vertStart.x, vertStart.y + 0.05f);
                    myObj->objBody->GetWorld()->RayCast(&rayData, vertStart, vertEnd);
                    if (rayData.outHit) {
                        grounded = true;
                        return;
                    }
                }
            } else {
                SDL_Log("IsGrounded currently only supports polygon and chain shapes");
                return;
            }
        }
    }

    /*
    * These functions influence the world and game objects.
    * First off are the functions for moving and rotating a game object.
    * Next, are the game state functions for winning, loosing, and reseting the game.
    * Finally there are functions for destroying either the current game object, or the one saved in the touch gObj register.
    */

    /*
    Movement based functions
    */

    //Applies a force, in the world space x and y directions specified, to the body calling Push().
    void Push(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        myObj->objBody->ApplyLinearImpulseToCenter(data, true);
    }
    //Sets the velocity for a single frame, in the world space x and y directions specified, the the game object calling Velocity().
    //This will override and reset gravitational force.
    void Velocity(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        myObj->objBody->SetLinearVelocity(data);
    }
    //Applies a force, in the local space x and y directions specified, to the body calling Push().
    void PushLocal(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        float rot = myObj->objBody->GetAngle();
        b2Vec2 rotVector = b2Vec2(data.x * cos(rot) - data.y * sin(rot), data.x * sin(rot) + data.y * cos(rot));
        myObj->objBody->ApplyLinearImpulseToCenter(rotVector, true);
    }
    //Sets the velocity for a single frame, in the local space x and y directions specified, the the game object calling Velocity().
    //This will override and reset gravitational force.
    void VelocityLocal(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        float rot = myObj->objBody->GetAngle();
        b2Vec2 rotVector = b2Vec2(data.x * cos(rot) - data.y * sin(rot), data.x * sin(rot) + data.y * cos(rot));
        myObj->objBody->SetLinearVelocity(rotVector);
    }
    //Teleports the game object to the world space x and y coordinates specified.
    void Teleport(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        float rot = myObj->objBody->GetAngle();
        myObj->objBody->SetTransform(data, rot);
    }

    /*
    Rotation based functions
    */

    //Adds rotational velocity to the game object in a clockwise direction (in radians)
    void Rotate(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        myObj->objBody->SetAngularVelocity(data.x);
    }

    /*
    Destruction functions
    */
    //removes the current object from the game
    void DestroySelf(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        EngineManager::QueueObjectToDestroy(myObj);
    }
    //destroies the object in the touchObj resister, then resets the register
    void DestroyTouch(bool *conditional, bool notted, void* gObj, b2Vec2 data) {
        if (!(*conditional ^ notted)) return;
        GameObject* myObj = (GameObject*) gObj;
        /*
        int result = MessageBox(
            NULL,
            "You Win!",
            "Victory",
            MB_OK
        );

        if (result == IDOK) {
            //reset the game or something
        }
        */
        if (touchObj != nullptr) EngineManager::QueueObjectToDestroy(touchObj);
        touchObj = nullptr;
    }

    /*
    Game State based functions
    */
};