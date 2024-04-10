#pragma once
//Custom scritps
#include "Brush.h"
#include "GameObject.h"

//SDL and ImGui database
#include "imgui/imgui.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_image.h"

//Box2D is the physics library
#include "box2d/box2d.h"

namespace Inspector {
    const char* bodyTypes[3] = {"Static", "Dynamic", "Kinematic"};
    const char* bodyShapes[4] = {"None", "Ecllipse", "Box", "Polygon"};

    void BuildPhysicsBodySelector(GameObject* selectedObject) {
        const char* currentBodyType;
        switch(selectedObject->objBodyDef.type) {
            default:
                currentBodyType = "None";
                break;
            case b2BodyType::b2_dynamicBody:
                currentBodyType = "Dynamic";
                break;
            case b2BodyType::b2_kinematicBody:
                currentBodyType = "Kinematic";
                break;
            case b2BodyType::b2_staticBody:
                currentBodyType = "Static";
                break;
        }
        if (ImGui::BeginCombo("Physics Type:", currentBodyType)) {
            for (int i = 0; i < 3; i++) {
                bool is_selected = (currentBodyType == bodyTypes[i]);
                if (ImGui::Selectable(bodyTypes[i], is_selected)) {
                    currentBodyType = bodyTypes[i];
                    switch(i) {
                        default:
                        case 0:
                            selectedObject->objBodyDef.type = b2BodyType::b2_staticBody;
                            break;
                        case 1:
                            selectedObject->objBodyDef.type = b2BodyType::b2_dynamicBody;
                            break;
                        case 2:
                            selectedObject->objBodyDef.type = b2BodyType::b2_kinematicBody;
                            break;
                    }
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }
    
    void BuildShapeSelector(GameObject* selectedObject) {
        const char* currentBodyShape;
        switch(selectedObject->objShape) {
            case 0:
                currentBodyShape = "None";
                break;
            case 1:
                currentBodyShape = "Ecllipse";
                break;
            case 2:
                currentBodyShape = "Box";
                break;
            case 3:
                currentBodyShape = "Polygon";
                break;
        }
        if (ImGui::BeginCombo("Physics Body Shape:", currentBodyShape)) {
            for (int i = 0; i < 4; i++) {
                bool is_selected = (currentBodyShape == bodyShapes[i]);
                if (ImGui::Selectable(bodyShapes[i], is_selected)) {
                    currentBodyShape = bodyShapes[i];
                    selectedObject->objShape = (GameObject::Shape) i;
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }

    void BuildPlayModeInspector(GameObject* selectedObject) {
        std::string xPos = "X Pos: " + std::to_string(selectedObject->objBody->GetPosition().x);
        ImGui::Text(xPos.c_str());
        std::string yPos = "Y Pos: " + std::to_string(selectedObject->objBody->GetPosition().y);
        ImGui::Text(yPos.c_str());
        std::string angle = "Angle: " + std::to_string(selectedObject->objBody->GetAngle());
        ImGui::Text(angle.c_str());
        std::string width = "Width: " + std::to_string(selectedObject->width);
        ImGui::Text(width.c_str());
        std::string height = "Height: " + std::to_string(selectedObject->height);
        ImGui::Text(height.c_str());
    }

    void BuildNormalInspector(GameObject* selectedObject, std::vector<GameObject*>* currObjects, SDL_Renderer* renderer) {
        float xPos = selectedObject->objBodyDef.position.x;
        float yPos = selectedObject->objBodyDef.position.y;
        ImGui::SliderFloat("X Pos", &xPos, 0, 50);
        ImGui::SliderFloat("Y Pos", &yPos, 0, 20);
        ImGui::SliderFloat("Angle", &selectedObject->objBodyDef.angle, 0, 2 * b2_pi);
        ImGui::SliderFloat("Width", &selectedObject->width, 0.1f, 20);
        ImGui::SliderFloat("Height", &selectedObject->height, 0.1f, 20);
        if (ImGui::SliderInt("Layer", &selectedObject->layer, 0, 7)) {
            bool found = false;
            for (Uint8 i = 0; i < 8; i++) {//(std::vector<GameObject*> &layer : currObjects) {
                for (Uint8 k = 0; k < currObjects[i].size(); k++) {
                    if (*currObjects[i][k] == *selectedObject) {
                        currObjects[selectedObject->layer].push_back(selectedObject);
                        //std::cout << "Moved to Layer: " + std::to_string(selectedObject->layer) + " - New Layer Size: " + std::to_string(currObjects[selectedObject->layer].size()) << std::endl;
                        selectedObject->SetTextureFromeFile(renderer, selectedObject->GetFilePath());
                        currObjects[i].erase(currObjects[i].begin() + k);
                        found = true;
                        break;
                    }
                }
                if (found) break;
            }
        }
        ImGui::SliderFloat("Density", &selectedObject->density, 0.01f, 10);
        ImGui::SliderFloat("Friction", &selectedObject->friction, 0, 10);
        selectedObject->objBodyDef.position.Set(xPos, yPos);
        
        BuildPhysicsBodySelector(selectedObject);
        BuildShapeSelector(selectedObject);

        if (selectedObject->objShape == GameObject::Polygon) {
            ImGui::Text("Polygon Verts:");
            if (ImGui::Button("Add Vert")) {
                if (selectedObject->verts.size() < 8) {
                    selectedObject->verts.push_back(b2Vec2(0,0));
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Del Vert")) {
                if (selectedObject->verts.size() > 3) {
                    selectedObject->verts.erase(selectedObject->verts.end() - 1);
                }
            }
            ImGui::PushItemWidth(70.0f);
            for (Uint8 i = 0; i < selectedObject->verts.size(); i++) {
                ImGui::Text("(");
                ImGui::SameLine();
                ImGui::SliderFloat(("##X" + std::to_string(i)).c_str(), &selectedObject->verts[i].x, -5.0f, 5.0f, "%.1f");
                ImGui::SameLine();
                ImGui::Text(",");
                ImGui::SameLine();
                ImGui::SliderFloat(("##Y" + std::to_string(i)).c_str(), &selectedObject->verts[i].y, -5.0f, 5.0f, "%.1f");
                ImGui::SameLine();
                ImGui::Text(")");
            }
            ImGui::PopItemWidth();
        }

        ImGui::Text("Color:");
        int colorR = selectedObject->color.r;
        int colorG = selectedObject->color.g;
        int colorB = selectedObject->color.b;
        int colorA = selectedObject->color.a;
        if (ImGui::SliderInt("R", &colorR, 0, 255)) {
            selectedObject->color.r = colorR;
        }
        if (ImGui::SliderInt("G", &colorG, 0, 255)) {
            selectedObject->color.g = colorG;
        }
        if (ImGui::SliderInt("B", &colorB, 0, 255)) {
            selectedObject->color.b = colorB;
        }
        if (ImGui::SliderInt("A", &colorA, 0, 255)) {
            selectedObject->color.a = colorA;
        }
    }
}