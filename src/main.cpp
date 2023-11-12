#include <iostream>
#include <vector>
#include <exception>
#include <filesystem>
#include <algorithm>

#include "pako.h"
#include "db/database.h"

Pako main;
Database idb;

std::vector<std::string> arguments;

int main(int argc, char* argv[]) {
    try {
        if(!std::filesystem::is_directory((std::string)VAR_PATH + "/control/"))
            std::filesystem::remove_all((std::string)VAR_PATH + "/control/");
            std::filesystem::create_directories((std::string)VAR_PATH + "/control/");
        if(!std::filesystem::is_regular_file((std::string)VAR_PATH + "/packages.db/"))
            idb.initDatabase((std::string)VAR_PATH + "/packages.db");
    } catch (const std::exception& e) {
        out.errormsg("Can't init database, try running pako as superuser.");
        out.debugmsg("initDatabase: " + (std::string)e.what());
    }
    for(int i=1; i<argc; i++)
        arguments.push_back(std::move(argv[i]));
    if(std::find(arguments.begin(), arguments.end(), "-h") != arguments.end() || arguments.size() < 1) {
        main.help();
        return 0;
    }
    else if(std::find(arguments.begin(), arguments.end(), "-i") != arguments.end()) {
        arguments.erase(std::find(arguments.begin(), arguments.end(), "-i"));
        return(main.install(arguments));
    }
    else if(std::find(arguments.begin(), arguments.end(), "-r") != arguments.end()) {
        arguments.erase(std::find(arguments.begin(), arguments.end(), "-r"));
        return(main.remove(arguments));
    }
    else if(std::find(arguments.begin(), arguments.end(), "-l") != arguments.end()) {
        arguments.erase(std::find(arguments.begin(), arguments.end(), "-l"));
        return(main.list(arguments));
    }
    else if(std::find(arguments.begin(), arguments.end(), "-v") != arguments.end()) {
        main.version();
        return 0;
    } else {
        out.errormsg("Unknown action '" + arguments[0] + "'. Use -h for help.");
    }
    return 0;
}
