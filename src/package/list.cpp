#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "../common/config.h"
#include "../common/output.h"
#include "../db/database.h"

using std::vector, std::string, std::cout;

int List(vector<string> packages) {
    auto it = packages.begin();

    while (it != packages.end()) {
        const auto& packageit = *it;

        if (!db.IsIn(packageit)) {
            output.warn(packageit + " is not installed.");
            it = packages.erase(it);
        }
        else {
            ++it;
        }
    }
    if(packages.size() > 0) {

        for(const auto& packageit : packages) {
            Package package = db.GetPackage(packageit);
            cout << package.name << " " << package.version << " " << package.arch.name << " " << package.description << "\n";
        }
        return 0;
    }
    std::ifstream dbFile(configParams.dbPath);
    for(std::string line; std::getline(dbFile, line);) {
        Package package;
        std::istringstream iss(line);
        std::string key;
        iss >> key;

        if (key == "name") {
            iss >> package.name;
            package = db.GetPackage(package.name);
            cout << package.name << " " << package.version << " " << package.arch.name << " " << package.description << "\n";
        }
    }
    return 0;
}
