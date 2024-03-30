#include "GameObject.h"
#include <iostream>

GameObject::GameObject(SDL_Renderer* renderer, b2BodyDef targetBody, int shape, float targetWidth, float targetHeight, std::string textureFile, float targetDensity, float targetFriction) {
    objBodyDef = targetBody;
    width = targetWidth;
    height = targetHeight;
    density = targetDensity;
    friction = targetFriction;
    objShape = (shape <= 3) ? (Shape) shape : Box;
    SetTextureFromeFile(renderer, textureFile);
    SetUpdateFunction(DefaultUpdates::Empty);
}
GameObject::~GameObject() {
    if (targetTexture != nullptr || targetTexture != NULL) SDL_DestroyTexture(targetTexture);
    targetTexture = NULL;
    objBody = NULL;
}
GameObject* GameObject::Clone(SDL_Renderer* targetRenderer) {
    b2BodyDef newBody;
    newBody.position.Set(objBodyDef.position.x, objBodyDef.position.y);
    newBody.type = objBodyDef.type;
    newBody.angle = objBodyDef.angle;
    GameObject* newObj = new GameObject(targetRenderer, newBody, objShape, width, height, GetFilePath());
    newObj->SetTextureFromeFile(targetRenderer, newObj->GetFilePath());
    void (*objUpdateFunction)(void* gObj) = GetUpdateFunction();
    newObj->SetUpdateFunction(objUpdateFunction);
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
        objBody->CreateFixture(&boxFixtureDef);
    } else if (objShape == Ecllipse) {
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
        objBody->CreateFixture(&FixtureDef);

        //chain made for ellipse collisions
        b2ChainShape chainShape;
        chainShape.CreateLoop(verts, STEPS);
        b2FixtureDef triFixtureDef;
        triFixtureDef.shape = &chainShape;
        triFixtureDef.density = density;
        triFixtureDef.friction = friction;
        objBody->CreateFixture(&triFixtureDef);
    } else if (objShape == Polygon) {
        b2Vec2 points[3] = {b2Vec2(0, -0.5f), b2Vec2(0.5f, 0.5f), b2Vec2(-0.5f, 0.5f)};
        for (int i = 0; i < 3; i++) {
            points[i].x *= width;
            points[i].y *= height;
        }
        objBody = phyWorld->CreateBody(&objBodyDef);
        b2PolygonShape polygon;
        polygon.Set(points, 3);
        b2FixtureDef polygonFixtureDef;
        polygonFixtureDef.shape = &polygon;
        polygonFixtureDef.density = density;
        polygonFixtureDef.friction = friction;
        objBody->CreateFixture(&polygonFixtureDef);
    }
}

void GameObject::SetTextureFromeFile(SDL_Renderer* renderer, std::string textureFile) {
    textureFilePath = textureFile;
    targetTexture = IMG_LoadTexture(renderer, &textureFile[0]);
}