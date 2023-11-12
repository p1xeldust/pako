#include <filesystem>
#include <exception>

#include "package.h"

bool Package::mkTemp(std::filesystem::path tmpPath) {
    out.debugmsg("mkTemp: Trying to make temp dir: " + tmpPath.string());
    try {
        if(std::filesystem::exists(tmpPath))
            std::filesystem::remove_all(tmpPath);
        std::filesystem::create_directories(tmpPath);
    } catch(const std::exception& e) {
        out.debugmsg("mkTemp: Unable to create tmp dir: " + (std::string)e.what());
        return false;
    }
    return true;
}
