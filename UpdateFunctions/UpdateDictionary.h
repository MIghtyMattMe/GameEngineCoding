#pragma once
#include "../GameObject.h"
#include "box2d/box2d.h"
#include "KeyData.h"
#include "ScriptConverter.h"
#include <iostream>
#include <filesystem>
#include <map>

namespace UpdateDictionary {
    std::map<std::string, std::vector<void (*)(bool* conditional, bool notted, void* gObj, b2Vec2 data)>> UpdateFunctions = {};
    bool Generate() {
        //clear the old map
        UpdateFunctions.clear();
        //add in the default update functions
        for (std::filesystem::directory_entry const &file : std::filesystem::recursive_directory_iterator("/Default")) {
            std::string name;
            std::vector<void (*)(bool* conditional, bool notted, void* gObj, b2Vec2 data)> functionCalls = std::vector<void (*)(bool* conditional, bool notted, void* gObj, b2Vec2 data)>();
            if (!ScriptConverter::ConvertFile(file.path(), name, functionCalls)) return false;
            UpdateFunctions.insert({name, functionCalls});
        }
        //add in the custom update functions
        for (std::filesystem::directory_entry const &file : std::filesystem::recursive_directory_iterator("/Custom")) {
            std::string name;
            std::vector<void (*)(bool* conditional, bool notted, void* gObj, b2Vec2 data)> functionCalls = std::vector<void (*)(bool* conditional, bool notted, void* gObj, b2Vec2 data)>();
            if (!ScriptConverter::ConvertFile(file.path(), name, functionCalls)) return false;
            UpdateFunctions.insert({name, functionCalls});
        }
        return true;
    }
};