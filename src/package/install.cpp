#include <filesystem>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "../common/output.h"
#include "../common/root.h"
#include "../db/database.h"
#include "../common/copy.h"
#include "../common/config.h"
#include "../common/dialog.h"
#include "package.h"
#include "exec.h"
#include "unpack.h"
#include "parse.h"
#include "cleanup.h"

int Install(std::vector<std::string> packagesAsFiles)
{
    create_directories(configParams.varPath / "control");
    auto it = packagesAsFiles.begin();
    while (it != packagesAsFiles.end()) {
        const auto &packageFile = *it;
        if (!std::filesystem::is_regular_file(packageFile)) {
            output.warn(packageFile + " is an invalid file, skipping");
            it = packagesAsFiles.erase(it);
        }
        else ++it;
    }
    CheckRoot();
    for (const std::string packageFile : packagesAsFiles) {
        std::filesystem::path tmpPath = configParams.tmpPath / (packageFile + "_tmp");

        if (unpackPackage(packageFile) == -1) {
            output.warn("Can't install " + packageFile + ": bad package");
            CleanUpInstall(packageFile);
            continue;
        }
    }
    for (const std::string packageFile : packagesAsFiles) {
        std::filesystem::path tmpPath = configParams.tmpPath / (packageFile + "_tmp");
        Package package = ParseSpecs(tmpPath / "PAKO/info");
        if (package.skipcurrent) {
            output.warn("Skipping " + packageFile);
            CleanUpInstall(packageFile);
            continue;
        }
        if(std::filesystem::exists(tmpPath / "PAKO/install")) {
            package.files.scriptFilePath = configParams.varPath / "control" / (package.name + ".install");
            execScript(tmpPath / "PAKO/install", PRE_INSTALL);
            copy_file(tmpPath / "PAKO/install", package.files.scriptFilePath, std::filesystem::copy_options::update_existing);
        }
        if(package.meta == PACKAGE_DEFAULT) {
            package.files.listFilePath = configParams.varPath / "control" / (package.name + ".files");
            std::ifstream listFile(tmpPath / "PAKO/files");
            copyByList((tmpPath / "sources"), listFile, package);
            copy_file(tmpPath / "PAKO/files", package.files.listFilePath, std::filesystem::copy_options::update_existing);
        }
        db.AddPackage(package);
        CleanUpInstall(packageFile);
        output.msg("Installed " + package.name + " " + package.version + " (" + package.arch.name + ")");
    }
    return 0;
}
