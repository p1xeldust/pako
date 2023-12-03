#include <filesystem>
#include <exception>

#include "package.h"
#include "../output/print.h"

bool make_tmp(std::string path) {
    #ifdef DEBUG
    debugmsg("mkTemp: Trying to make temp dir: %s\n", path.c_str());
    #endif
    try {
        if(std::filesystem::exists((std::string)TMP_PATH + "/" + path))
            std::filesystem::remove_all((std::string)TMP_PATH + "/" + path);
        std::filesystem::create_directories((std::string)TMP_PATH + "/" + path);
    } catch(const std::exception& e) {
        errormsg("Can't create temporary folders.\n");
        #ifdef DEBUG
        debugmsg("mkTemp: Unable to create tmp dir: %s\n", e.what());
        #endif
        return false;
    }
    return true;
}

bool clear_tmp() {
    try {
        std::filesystem::remove_all((std::string)TMP_PATH);
    }
    catch(const std::exception& e) {
        errormsg("Can't clean temporary folders.\n");
        #ifdef DEBUG
        debugmsg("mkTemp: Unable to clean tmp dir: %s\n", e.what());
        #endif
        return false;
    }
    return true;
}
