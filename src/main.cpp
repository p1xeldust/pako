#include <iostream>
#include <vector>
#include <exception>
#include <filesystem>
#include <algorithm>

#include "pako.hpp"

std::vector<std::string> arguments;

int main(int argc, char* argv[]) {
    try {
        if(!std::filesystem::is_directory((std::string)VAR_PATH + "/control/"))
            std::filesystem::create_directories((std::string)VAR_PATH + "/control/");
        if(!std::filesystem::is_regular_file((std::string)VAR_PATH + "/packages.db"))
            initDatabase();
    } catch (const std::exception& e) {
        errormsg("Can't init database, try running pako as superuser.\n");
        debugmsg("initialSetup: %s\n",e.what());
    }
    for(int i=1; i<argc; i++)
        arguments.push_back(std::move(argv[i]));
    if(std::find(arguments.begin(), arguments.end(), "-h") != arguments.end() || arguments.size() < 1) {
        help();
        return 0;
    }
    else if(std::find(arguments.begin(), arguments.end(), "-i") != arguments.end()) {
        arguments.erase(std::find(arguments.begin(), arguments.end(), "-i"));
        return(installPackage(arguments));
    }
    else if(std::find(arguments.begin(), arguments.end(), "-r") != arguments.end()) {
        arguments.erase(std::find(arguments.begin(), arguments.end(), "-r"));
        return(removePackage(arguments));
    }
    else if(std::find(arguments.begin(), arguments.end(), "-l") != arguments.end()) {
        arguments.erase(std::find(arguments.begin(), arguments.end(), "-l"));
        return(listPackages(arguments));
    }
    else if(std::find(arguments.begin(), arguments.end(), "-v") != arguments.end()) {
        version();
        return 0;
    } else {
        errormsg("Unknown action '%s'. Use -h for help.",  arguments[0].c_str());
    }
    return 0;
}
