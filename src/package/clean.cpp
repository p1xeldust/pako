#include <filesystem>
#include "package.h"

using std::string, std::filesystem::remove_all;

int cleanUpInstall(string packageFile) {
    return remove_all((string)TMP_PATH + "/" + packageFile + "_tmp/");
}