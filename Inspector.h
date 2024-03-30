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

    void BuildNormalInspector(GameObject* selectedObject) {
        float xPos = selectedObject->objBodyDef.position.x;
        float yPos = selectedObject->objBodyDef.position.y;
        ImGui::SliderFloat("X Pos", &xPos, 0, 50);
        ImGui::SliderFloat("Y Pos", &yPos, 0, 20);
        ImGui::SliderFloat("Angle", &selectedObject->objBodyDef.angle, 0, 2 * b2_pi);
        ImGui::SliderFloat("Width", &selectedObject->width, 0.1f, 20);
        ImGui::SliderFloat("Height", &selectedObject->height, 0.1f, 20);
        ImGui::SliderFloat("Density", &selectedObject->density, 0.01f, 10);
        ImGui::SliderFloat("Friction", &selectedObject->friction, 0, 10);
        selectedObject->objBodyDef.position.Set(xPos, yPos);
        
        BuildPhysicsBodySelector(selectedObject);
        BuildShapeSelector(selectedObject);
    }
}