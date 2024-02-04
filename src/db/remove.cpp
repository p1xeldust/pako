#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "../common/config.h"
#include "database.h"

void Database::RemovePackage(Package& package) {
    std::ifstream oldDB(configParams.dbPath);
    std::ofstream newDB(configParams.dbPath.string() + ".new");

    for(std::string line; std::getline(oldDB, line);) {
        if (line == "name " + package.name) {
            while(std::getline(oldDB, line) && line != "EOPE") {}
            continue;
        }
        newDB << line << std::endl;
    }
    oldDB.close();
    newDB.close();
    std::rename(configParams.dbPath.c_str(), (configParams.dbPath.string() + ".old").c_str());
    std::rename((configParams.dbPath.string() + ".new").c_str(), configParams.dbPath.c_str());
}
