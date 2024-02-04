#include <filesystem>

#include "package.h"

#include "../common/config.h"
#include "../common/tar.h"
#include "parse.h"

int unpackPackage(std::filesystem::path packageFile) {
    std::filesystem::path currentTmpPath = configParams.tmpPath / (packageFile.filename().string() + "_tmp");
    std::filesystem::create_directories(currentTmpPath);
    if (extractTar(packageFile, currentTmpPath) == -1)
        return -1;
    return 0;
}