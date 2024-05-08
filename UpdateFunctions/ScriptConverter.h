#pragma once
#include "../GameObject.h"
#include "box2d/box2d.h"
#include "KeyData.h"
#include "CoreFunctions.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <map>

namespace ScriptConverter {
    bool notRecord = false;
    bool *condition = &CoreUpdateFunctions::alwaysTrue;
    GameObject *currObj = nullptr;

    class functionHolder{
        public:
            void (*function)(bool&, bool, void*, b2Vec2);
            bool &conditional; 
            bool notted; 
            GameObject* gObj; 
            b2Vec2 data;
            functionHolder() {
                function = nullptr;
                conditional = notted;
                notted = false;
                gObj = nullptr;
                data = b2Vec2();
            }
    };

    bool ConvertFile(std::filesystem::path path, std::string &name, std::vector<functionHolder> &functionCalls) {
        //first, load the file
        std::ifstream srcFile(path.string());
        std::string input;
        //then, read code from the file
        while (std::getline(srcFile, input)) {
            if (!ConvertLine(input, functionCalls)) return false;
        }
        //reset conditionals
        condition = &CoreUpdateFunctions::alwaysTrue;
        notRecord = false;
        return true;
    }
    bool ConvertLine(std::string line, std::vector<functionHolder> &functionCalls) {
        size_t lineIndex = 0;
        std::string lineSegment = "";
        std::vector<std::string> lineSegments = std::vector<std::string>();
        while ((lineIndex = line.find(' ')) != std::string::npos) {
            lineSegment = line.substr(0, line.find(':'));
            lineSegments.push_back(lineSegment);
            line.erase(0, lineIndex + 1);
        }
        if (lineSegments.size() <= 0) {
            SDL_Log("Update function has an empty line.");
            return false;
        }
        //based on the name and push the function to the call stack
        if (lineSegments[0].compare("EXIT") == 0) {
            //reset conditionals
            condition = &CoreUpdateFunctions::alwaysTrue;
            notRecord = false;
        } else if (lineSegments[0].compare("IF") == 0) {
            if (lineSegments.size() <= 1 || lineSegments.size() > 3) {
                SDL_Log("IF and IF NOT should only have 1 argument");
                return false;
            }

            //set notRecord
            int index = 1;
            if (lineSegments[1].compare("NOT") == 0) {
                index = 2;
                notRecord = true;
            } else {
                notRecord = false;
            }

            //find the right conditional
            if (lineSegments[index].compare("key") == 0) {
                condition = &CoreUpdateFunctions::key;
            } else if (lineSegments[index].compare("touch") == 0) {
                condition = &CoreUpdateFunctions::touch;
            }
        } else if (lineSegments[0].compare("Push") == 0) {
            if (lineSegments.size() != 3) {
                SDL_Log("Push command only takes 2 arguments.");
                return false;
            }
            functionHolder *newFunc = new functionHolder();
            newFunc->function = CoreUpdateFunctions::Push;
            newFunc->conditional = &(*condition);
            newFunc->notted = (notRecord) ? true : false;
            newFunc->gObj = &(*currObj);
            newFunc->data.x = std::stof(lineSegments[1]);
            newFunc->data.y = std::stof(lineSegments[2]);
            functionCalls.push_back(*newFunc);
        } else if (lineSegments[0].compare("KeyPressed") == 0) {
            if (lineSegments.size() != 2) {
                SDL_Log("Push command only takes 1 arguments.");
                return false;
            }
            functionHolder *newFunc = new functionHolder();
            newFunc->function = CoreUpdateFunctions::Push;
            newFunc->conditional = &(*condition);
            newFunc->notted = (notRecord) ? true : false;
            newFunc->gObj = &(*currObj);
            newFunc->data.x = lineSegments[1][0];
            newFunc->data.y = 0;
            functionCalls.push_back(*newFunc);
        } else {
            std::string logMsg = "Update function has unidentified command: " + lineSegments[0];
            SDL_Log(logMsg.c_str());
            return false;
        }
        return true;
    }
};