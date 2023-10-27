#include <iostream>
#include <vector>
#include <exception>
#include <filesystem>
#include <algorithm>

extern "C" void help();
extern "C" void version();
extern "C" void debugmsg(const char* message);
extern "C" void errormsg(const char* message);
extern int8_t Install(std::vector<std::string> arguments);
extern int8_t Remove(std::vector<std::string> arguments);
extern int8_t List(std::vector<std::string> arguments);

std::vector<std::string> arguments;

int main(int argc, char* argv[]) {
    try {
        if(!std::filesystem::is_directory((std::string)VAR_PATH + "/packages/"))
            std::filesystem::create_directories((std::string)VAR_PATH + "/packages/");
    } catch (const std::exception& e) {
        errormsg("Enable to create packages list. Try running pako as superuser.");
        debugmsg(("[CreateVar] " + (std::string)e.what()).c_str());
    }
    for(int i=1; i<argc; i++)
        arguments.push_back(std::move(argv[i]));

    if(std::find(arguments.begin(), arguments.end(), "-h") != arguments.end() || arguments.size() < 1) {
        help();
        return 0;
    }
    else if(std::find(arguments.begin(), arguments.end(), "-i") != arguments.end()) {
        arguments.erase(std::find(arguments.begin(), arguments.end(), "-i"));
        return(Install(arguments));
    }
    else if(std::find(arguments.begin(), arguments.end(), "-r") != arguments.end()) {
        arguments.erase(std::find(arguments.begin(), arguments.end(), "-r"));
        return(Remove(arguments));
    }
    else if(std::find(arguments.begin(), arguments.end(), "-l") != arguments.end()) {
        arguments.erase(std::find(arguments.begin(), arguments.end(), "-l"));
        return(List(arguments));
    }
    else if(std::find(arguments.begin(), arguments.end(), "-v") != arguments.end()) {
        version();
        return 0;
    } else {
        std::cout << "[Error] Unknown option '" << arguments[0] << "'"  << std::endl;
    }
    return 0;
}
