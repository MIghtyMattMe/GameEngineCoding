#include "Default.h"
class RayCastCallback : public b2RayCastCallback {
    public:
        RayCastCallback() { outHit = false; }
        virtual float ReportFixture(b2Fixture* fix, const b2Vec2& pnt, const b2Vec2& norm, float frac) {
            outHit = true;
            outPoint = pnt;
            outFixture = fix;
            outNorm = norm;
            outFraction = frac;
            return 0;
        }
        bool outHit;
        b2Fixture* outFixture;
        b2Vec2 outPoint;
        b2Vec2 outNorm;
        float outFraction;
};

namespace DefaultUpdates {
    void* functionList[2] = {Empty, MovePlayer};
    int GetFunctionNumber(void (*function)(void* gObj)) {
        for (int i = 0; i < 2; i++) {
            if (functionList[i] == function) {
                return i;
            }
        }
        return 0;
    }
    void (*GetFunctionFromNumber(int num))(void*) {
        return (void(*)(void* gObj))functionList[num];
    }
    
    void Empty(void* gObj) {
        //do nothing
    }
    void MovePlayer(void* gObj) {
        GameObject* playerObj = (GameObject*) gObj;
        if (playerObj->objBody->GetType() != b2_dynamicBody) return;
        RayCastCallback RayData = RayCastCallback();
        b2Vec2 floorPoint = b2Vec2(playerObj->objBody->GetPosition().x, playerObj->objBody->GetPosition().y + (playerObj->height / 2));
        b2Vec2 endPoint = b2Vec2(floorPoint.x, floorPoint.y + 0.05f);
        playerObj->objBody->GetWorld()->RayCast(&RayData, floorPoint, endPoint);
        b2Vec2 impulse = b2Vec2(0.0f, 0.0f);
        playerObj->objBody->SetFixedRotation(true);
        if (KeyData::KeyJustPressed(SDL_SCANCODE_W) && RayData.outHit) {
            impulse.y -= 8;
        }
        if (KeyData::KeyDown(SDL_SCANCODE_D)) {
            impulse.x += 5;
        }
        if (KeyData::KeyDown(SDL_SCANCODE_A)) {
            impulse.x -= 5;
        }
        if (KeyData::KeyDown(SDL_SCANCODE_S)) {
            //impulse.y += 8;
        }
        playerObj->objBody->SetLinearVelocity(b2Vec2(0, playerObj->objBody->GetLinearVelocity().y));
        playerObj->objBody->SetAngularVelocity(0);
        playerObj->objBody->ApplyLinearImpulseToCenter(impulse, true);
    }
};