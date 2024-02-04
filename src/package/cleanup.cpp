#include <filesystem>

#include "../common/config.h"
#include "cleanup.h"

int CleanUpInstall(std::filesystem::path packageFile) {
    return remove_all(configParams.tmpPath / (packageFile.filename().string() + "_tmp"));
}