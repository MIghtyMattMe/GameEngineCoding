#pragma once
#include "../GameObject.h"
#include "box2d/box2d.h"
#include "KeyData.h"
#include "ScriptConverter.h"
#include <iostream>
#include <filesystem>
#include <map>

namespace UpdateDictionary {
    extern std::map<std::string, std::vector<ScriptConverter::functionHolder>> UpdateFunctions = {};
    bool Generate() {
        //clear the old map
        UpdateFunctions.clear();
        UpdateFunctions.insert({"None", std::vector<ScriptConverter::functionHolder>()});
        //add in the default update functions
        for (std::filesystem::directory_entry const &file : std::filesystem::recursive_directory_iterator("UpdateFunctions/Default")) {
            std::string name;
            std::vector<ScriptConverter::functionHolder> functionCalls = std::vector<ScriptConverter::functionHolder>();
            std::cout << "begin file conversion for Defaults" << std::endl;
            if (!ScriptConverter::ConvertFile(file.path(), name, functionCalls)) return false;
            std::cout << "finished file conversion for " << name << std::endl;
            UpdateFunctions.insert({name, functionCalls});
        }
        //add in the custom update functions
        for (std::filesystem::directory_entry const &file : std::filesystem::recursive_directory_iterator("UpdateFunctions/Custom")) {
            std::string name;
            std::vector<ScriptConverter::functionHolder> functionCalls = std::vector<ScriptConverter::functionHolder>();
            std::cout << "begin file conversion for Customs" << std::endl;
            if (!ScriptConverter::ConvertFile(file.path(), name, functionCalls)) return false;
            std::cout << "finished file conversion for " << name << std::endl;
            UpdateFunctions.insert({name, functionCalls});
        }
        return true;
    }
};