#include <filesystem>
#include <exception>

#include "package.h"

bool Package::make_tmp(std::string path) {
    out.debugmsg("mkTemp: Trying to make temp dir: " + path);
    try {
        if(std::filesystem::exists((std::string)TMP_PATH + "/" + path))
            std::filesystem::remove_all((std::string)TMP_PATH + "/" + path);
        std::filesystem::create_directories((std::string)TMP_PATH + "/" + path);
    } catch(const std::exception& e) {
        out.errormsg("Can't create temporary folders.");
        out.debugmsg("mkTemp: Unable to create tmp dir: " + (std::string)e.what());
        return false;
    }
    return true;
}

bool Package::clear_tmp() {
    try {
        std::filesystem::remove_all((std::string)TMP_PATH);
    }
    catch(const std::exception& e) {
        out.errormsg("Can't clean temporary folders.");
        out.debugmsg("mkTemp: Unable to clean tmp dir: " + (std::string)e.what());
        return false;
    }
    return true;
}
