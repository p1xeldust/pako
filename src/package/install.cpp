#include <filesystem>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "../essential/o.h"
#include "../db/database.h"
#include "copyByList.h"
#include "package.h"
#include "exec.h"
#include "unpack.h"
#include "clean.h"

#include "../pako.h"

extern Output output;
extern Database db;

using std::string, std::cout, std::vector, std::ifstream, 
        std::filesystem::path, std::filesystem::copy, 
        std::filesystem::create_directories, std::filesystem::is_regular_file, 
        std::remove, std::filesystem::exists;

int Pako::install(vector<string> packagesAsFiles) {
    create_directories((string)VAR_PATH + "/control/");
    auto it = packagesAsFiles.begin();

    while (it != packagesAsFiles.end()) {
        const auto& packageFile = *it;
        if (!is_regular_file(packageFile)) {
            output.warn(packageFile + " is an invalid file, skipping");
            it = packagesAsFiles.erase(it);
        }
        else {
            ++it;
        }
    }

    for (const auto& packageFile : packagesAsFiles) {
        string tmpPath = (string)TMP_PATH + "/" + path(packageFile).filename().string() + "_tmp/";
        Package package;

        if (unpackPackage(packageFile, package, tmpPath) == -1) {
            output.warn("Can't install " + packageFile + ". Cleaning up and skipping.");
            cleanUpInstall(path(packageFile).filename().string());
            continue;
        }

        ifstream listFile(tmpPath + "/PAKO/files");
        if(exists(tmpPath + "/PAKO/install"))
            execScript(package.files.installScript, PRE_INSTALL);
        copyByList(tmpPath + "/sources", listFile, package);
        package.files.listFile = (string)VAR_PATH + "/control/" + package.name + ".files";
        package.files.specFile = (string)VAR_PATH + "/control/" + package.name + ".info";
        package.files.installScript = (string)VAR_PATH + "/control/" + package.name + ".install";
        if(exists(tmpPath + "/PAKO/install")) {
            copy_file(tmpPath + "/PAKO/install", package.files.installScript, std::filesystem::copy_options::update_existing);
        }
        db.add(package);
        copy_file(tmpPath + "/PAKO/info", package.files.specFile, std::filesystem::copy_options::update_existing);
        copy_file(tmpPath + "/PAKO/files", package.files.listFile, std::filesystem::copy_options::update_existing);

        output.msg("Installed " + package.name);
        execScript(tmpPath + "/PAKO/install", POST_INSTALL);
        cleanUpInstall(path(packageFile).filename());
    }

    return 1;
}
