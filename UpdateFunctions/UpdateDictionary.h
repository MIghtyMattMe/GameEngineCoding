#pragma once
#include "../GameObject.h"
#include "box2d/box2d.h"
#include "KeyData.h"
#include "ScriptConverter.h"
#include <iostream>
#include <filesystem>
#include <map>

namespace UpdateDictionary {
    std::map<std::string, std::vector<ScriptConverter::functionHolder>> UpdateFunctions = {};
    bool Generate() {
        //clear the old map
        UpdateFunctions.clear();
        //add in the default update functions
        for (std::filesystem::directory_entry const &file : std::filesystem::recursive_directory_iterator("/Default")) {
            std::string name;
            std::vector<ScriptConverter::functionHolder> functionCalls = std::vector<ScriptConverter::functionHolder>();
            if (!ScriptConverter::ConvertFile(file.path(), name, functionCalls)) return false;
            UpdateFunctions.insert({name, functionCalls});
        }
        //add in the custom update functions
        for (std::filesystem::directory_entry const &file : std::filesystem::recursive_directory_iterator("/Custom")) {
            std::string name;
            std::vector<ScriptConverter::functionHolder> functionCalls = std::vector<ScriptConverter::functionHolder>();
            if (!ScriptConverter::ConvertFile(file.path(), name, functionCalls)) return false;
            UpdateFunctions.insert({name, functionCalls});
        }
        return true;
    }
};