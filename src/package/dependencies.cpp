#include <filesystem>
#include <fstream>
#include <string>

#include "../common/config.h"
#include "../common/output.h"
#include "../db/database.h"
#include "package.h"

#include "dependencies.h"

int isInTmp(std::string PackageName) {
    for (const std::filesystem::directory_entry tmpDir : std::filesystem::directory_iterator(configParams.tmpPath)) {
        std::ifstream specFile(tmpDir.path() / "PAKO/info");
        std::string line;
        std::getline(specFile, line);
        if(line == "name " + PackageName)
            return 1;
    }
    return 0;
}

Package CheckConflicts(Package& package) {
    bool hasConflict = false;

    for (const std::string& conflict : package.conflicts) {
        if (db.IsIn(conflict) || isInTmp(conflict)) {
            output.error("Package '" + conflict + "' conflicts with " + package.name + ".");
            package.skipcurrent = 1;
            hasConflict = true;
        }
    }

    if (!hasConflict) {
        std::ifstream dbFile(configParams.dbPath);
        std::string conflict;

        for (std::string line; std::getline(dbFile, line);) {
            std::istringstream iss(line);
            std::string word;

            while (iss >> word) {
                if (word.substr(0, word.find_last_of(" ")) == "name") {
                    conflict = word.substr(word.find_last_of(" ") + 1);
                    continue;
                }

                if (word == "conflicts") {
                    size_t pos = line.find(package.name);

                    if (pos != std::string::npos) {
                        output.error("Package '" + conflict + "' conflicts with " + package.name + ".");
                        package.skipcurrent = 1;
                        hasConflict = true;
                    }
                }
            }
        }
    }

    return package;
}

Package CheckDependencies(Package& package) {
    for (std::string dependency : package.dependencies) {
        if (!db.IsIn(dependency) && !isInTmp(dependency)) {
            output.error("Package '" + dependency + "' is required for " + package.name + ". Install it first.");
            package.skipcurrent = 1;
        }
    }
    return CheckConflicts(package);
}