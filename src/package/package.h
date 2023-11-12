#include <filesystem>

#include "../db/database.h"
#include "../output/print.h"

#ifndef PACKAGE_H
#define PACKAGE_H

class Package {
private:
    Print out;
    Database db;
public:
    bool mkTemp(std::filesystem::path tmpPath);
    bool unpack(std::string filePath, std::string destinationPath);
    bool readPackageData(std::string dataFilePath, std::string packageData[3]);
    bool checkDeps(std::string dataFilePath);
    bool checkConflicts(std::string dataFilePath);
    bool checkArch(std::string packageArch);
    void removePackageSource(std::string listFilePath);
    bool solveDeps(std::string packageName);
};

#endif
