#include <iostream>
#include <vector>
#include <exception>
#include <filesystem>
#include <algorithm>

#ifdef _WIN32
    #error "Windows is unavailable now"
#endif
#ifdef _android_
    #error "Android is unavailable now"
#endif

extern void Help();
extern void Version();
extern int8_t Install(std::vector<std::string> arguments);
extern int8_t Remove(std::vector<std::string> arguments);
extern int8_t List(std::vector<std::string> arguments);

std::vector<std::string> arguments;

void debugmsg(std::string msg) {
	#ifdef DEBUG
		std::cout << "\e[1;90m" << "[DEBUG] " << msg << "\e[1;0m" << std::endl;
	#endif
}

int main(int argc, char* argv[]) {
    try {
        if(!std::filesystem::is_directory((std::string)VAR_PATH + "/packages/"))
            std::filesystem::create_directories((std::string)VAR_PATH + "/packages/");
    } catch (const std::exception& e) {
        std::cout << "Unable to create packages list. Try running pako as superuser." << std::endl;
        debugmsg("[DEBUG:createVar] " + (std::string)e.what());
    }
    for(int i=1; i<argc; i++)
        arguments.push_back(std::move(argv[i]));
    if(std::find(arguments.begin(), arguments.end(), "-i") != arguments.end()) {
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
    else for(uint8_t i=0; i<4; i++)
        if(std::find(arguments.begin(), arguments.end(), "-h") != arguments.end() || arguments.size() < 1) {
            Help();
            break;
        }
    for(uint8_t i=0; i<4; i++)
        if(std::find(arguments.begin(), arguments.end(), "-v") != arguments.end()) {
            Version();
            break;
        }
    return 0;
}
