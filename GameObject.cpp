#include "GameObject.h"
#include <iostream>

GameObject::GameObject() {}
GameObject::GameObject(SDL_Renderer* renderer, b2BodyDef targetBody, int shape, float targetWidth, float targetHeight, SDL_Texture *texturePtr, float targetDensity, float targetFriction) {
    objBodyDef = targetBody;
    width = targetWidth;
    height = targetHeight;
    density = targetDensity;
    friction = targetFriction;
    objShape = (shape <= 3) ? (Shape) shape : Polygon;
    targetTexture = texturePtr;
    UpdateFunction = "None";
    tag = 0;
}
GameObject::~GameObject() {
    targetTexture = NULL;
    objBody = NULL;
}
GameObject* GameObject::Clone(SDL_Renderer* targetRenderer) {
    b2BodyDef newBody;
    newBody.position.Set(objBodyDef.position.x, objBodyDef.position.y);
    newBody.type = objBodyDef.type;
    newBody.angle = objBodyDef.angle;
    newBody.fixedRotation = objBodyDef.fixedRotation;
    GameObject* newObj = new GameObject(targetRenderer, newBody, objShape, width, height, targetTexture);
    newObj->UpdateFunction = UpdateFunction;
    newObj->layer = layer;
    newObj->tag = tag;
    newObj->color = color;
    newObj->verts = verts;
    return newObj;
}
void GameObject::CreateAndPlaceBody(b2World* phyWorld) {
    if (objShape == Box) {
        objBody = phyWorld->CreateBody(&objBodyDef);
        b2PolygonShape box;
        box.SetAsBox(width / 2, height / 2);
        b2FixtureDef boxFixtureDef;
        boxFixtureDef.shape = &box;
        boxFixtureDef.density = density;
        boxFixtureDef.friction = friction;
        boxFixtureDef.filter.categoryBits = (1 << layer);
        boxFixtureDef.filter.maskBits = (1 << layer);
        objBody->CreateFixture(&boxFixtureDef);
    } else if (objShape == Ellipse) {
        const int STEPS = 32;
        float a = (width >= height) ? width / 2 : height / 2;
        float b = (width >= height) ? height / 2 : width / 2;
        b2Vec2 verts[STEPS];
        for (int i = 0; i < STEPS; i++) {
            float t = (float) (i * 2 * b2_pi) / STEPS;
            verts[i] = b2Vec2((width / 2) * cosf(t), (height / 2) * sinf(t));
        }
        objBody = phyWorld->CreateBody(&objBodyDef);

        //circle made for normal collisions
        b2CircleShape cir;
        cir.m_radius = b;
        b2FixtureDef FixtureDef;
        FixtureDef.shape = &cir;
        FixtureDef.density = density;
        FixtureDef.friction = friction;
        FixtureDef.filter.categoryBits = (1 << layer);
        FixtureDef.filter.maskBits = (1 << layer);
        FixtureDef.userData.pointer = tag;
        objBody->CreateFixture(&FixtureDef);

        //chain made for ellipse collisions
        b2ChainShape chainShape;
        chainShape.CreateLoop(verts, STEPS);
        b2FixtureDef triFixtureDef;
        triFixtureDef.shape = &chainShape;
        triFixtureDef.density = density;
        triFixtureDef.friction = friction;
        triFixtureDef.filter.categoryBits = (1 << layer);
        triFixtureDef.filter.maskBits = (1 << layer);
        objBody->CreateFixture(&triFixtureDef);
    } else if (objShape == Polygon) {
        std::vector<b2Vec2> polyPoints = verts;
        for (Uint8 i = 0; i < polyPoints.size(); i++) {
            polyPoints[i].x *= width;
            polyPoints[i].y *= height;
        }
        objBody = phyWorld->CreateBody(&objBodyDef);
        b2PolygonShape polygon;
        polygon.Set(polyPoints.data(), (int32) polyPoints.size());
        b2FixtureDef polygonFixtureDef;
        polygonFixtureDef.shape = &polygon;
        polygonFixtureDef.density = density;
        polygonFixtureDef.friction = friction;
        polygonFixtureDef.filter.categoryBits = (1 << layer);
        polygonFixtureDef.filter.maskBits = (1 << layer);
        objBody->CreateFixture(&polygonFixtureDef);
    } else if (objShape == None) {
        objBody = phyWorld->CreateBody(&objBodyDef);
    }
}