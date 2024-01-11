/*
    Dependencies and conflicts checks for package
    Written by Paul Goldstein, Jan 2024

*/

#include <filesystem>
#include <sqlite3.h>
#include <fstream>
#include <string>
#include <vector>

#include "../db/database.h"
#include "../essential/o.h"
#include "package.h"

extern Output output;

using std::string, std::filesystem::directory_iterator, std::getline, std::ifstream;

int isInTmp(string PackageName) {
    for (const auto tmpDir : directory_iterator((string)TMP_PATH)) {
        ifstream specFile((tmpDir.path() / "PAKO/info").string());
        string line;
        getline(specFile, line);
        output.msg(line);
        if(PackageName == line.substr(line.find(" ")+1))
            return 1;
    }
    return 0;
}

int CheckConflicts(Package& package) {
    int fail = 0;
    for (auto &conflict : package.conflicts) {
        if (db.isIn(conflict) && isInTmp(conflict)) {
            output.error("Package '" + conflict + "' conflicts with " + package.name + ".");
            fail = 1;
        }
    }
    if (fail)
        return -1;
    return 0;
}

int CheckDeps(Package& package) {
    int fail = 0;
    for (auto &dep : package.dependencies) {
        if (!db.isIn(dep) && !isInTmp(dep)) {
            output.error("Package '" + dep + "' is required for " + package.name + ". Install it first.");
            fail = 1;
        }
    }
    if (fail)
        return -1;
    return CheckConflicts(package);
}