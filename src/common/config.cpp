#include <filesystem>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>

#include "output.h"
#include "config.h"

ConfigParams configParams;

void ConfigParams::ReadConfigFile(const std::string& filePath) {
    std::ifstream configFile(filePath);

    /* Если файла нет или ошибка, используем предустановленные значения из config.h */
    if (!configFile.good()) {
        output.error("Can't open config file, exiting.");
        exit(EXIT_FAILURE);
    }

    for(std::string line; std::getline(configFile, line);) {
        if(line[0] == '#' || line.empty())
            continue;
        std::stringstream lineParameters(line);
        std::string type, value;
        if (!(lineParameters >> type) || type.empty() || !(lineParameters >> value) || value.empty())
            continue;
        if(type == "prefix")
            varPrefixPath = value;
        else if(type == "vardir")
            varVarPath = value;
        else if(type == "tmpdir")
            varTmpPath = value;
        else continue;
    }
    configFile.close();
    updateVariables();
}

void ConfigParams::updateVariables() {
    prefixPath.append(varPrefixPath + "/");
    tmpPath.append(varTmpPath + "/pako");
    varPath.append(prefixPath.string() + (varVarPath + "/pako"));
    dbPath = configParams.varPath / "packages";
}