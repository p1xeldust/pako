/*
    Package specs parser
    Written by Paul Goldstein, Jan 2024
*/

#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

#include "../common/output.h"
#include "package.h"
#include "specs.h"

extern Output output;

Package ParseSpecs(std::filesystem::path specFilePath)
{
    Package package;
    std::ifstream specFile(specFilePath);

    if (!specFile)
    {
        output.error("parse.cpp: spec file not found, skipping.");
        package.skipcurrent = 1;
        return package;
    }
    for (std::string line; std::getline(specFile, line);)
    {
        std::stringstream ss(line);
        std::string type, value;
        ss >> type;
        std::getline(ss >> std::ws, value);
        if (type == "name" && !value.empty())
            package.name = value;
        else if (type == "version" && !value.empty())
            package.version = value;
        else if (type == "arch" && !value.empty())
            package.arch.name = value;
        else if (type == "type" && !value.empty())
            package.meta = static_cast<packageMetaType>(stoi(value));
        else if (type == "dependencies" && !value.empty())
        {
            std::stringstream dependencies(value);
            for (std::string dependency; dependencies >> dependency;)
                package.dependencies.push_back(dependency);
        }
        else if (type == "conflicts" && !value.empty())
        {
            std::stringstream conflicts(value);
            for (std::string conflict; conflicts >> conflict;)
                package.conflicts.push_back(conflict);
        }
        else if (type == "description" && !value.empty())
            package.description = value;
        else
            continue;
    }
    return checkSpecs(package);
}