#pragma once
#include "../GameObject.h"
#include "box2d/box2d.h"
#include "KeyData.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <map>

namespace ScriptConverter {
    bool ConvertFile(std::filesystem::path path, std::string &name, std::vector<void (*)(bool* conditional, bool notted, void* gObj, b2Vec2 data)> &functionCalls) {
        //first, load the file
        std::ifstream srcFile(path.string());
        std::string input;
        //then, read new gameObjects from the file
        while (std::getline(srcFile, input)) {
            if (!ConvertLine(input, functionCalls)) return false;
        }
        return true;
    }
    bool ConvertLine(std::string line, std::vector<void (*)(bool* conditional, bool notted, void* gObj, b2Vec2 data)> &functionCalls) {
        return true;
    }
};