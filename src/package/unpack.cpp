/*
    Package file unpack function
    Written by Paul Goldstein, Jan 2024
*/
#include <filesystem>
#include <string>

#include "../essential/tar.h"
#include "parse.h"


using std::string, std::filesystem::path;

int unpackPackage(string packageFile, Package& package, string tmpPath) {
    if (extract_tar(packageFile, tmpPath) == -1)
        return -1;
    return parseSpecs(tmpPath + "/PAKO/info", package);
}