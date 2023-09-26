// Для простоты
#include <bits/stdc++.h>
#include <exception>
#include <filesystem>

#ifdef _WIN32
    #error "Windows compilation is unavailable now"
#endif
#ifdef _android_
    #error "Android is unavailable now"
#endif
extern void Help();
extern void Version();
extern int8_t Install(std::vector<std::string> arguments);
extern int8_t Remove(std::vector<std::string> arguments);
extern int8_t List(std::vector<std::string> arguments);

// Чтобы всем удобней было. Да, больше весит и выполняется чуууууууть дольше, но полезно
std::vector<std::string> arguments;

int main(int argc, char* argv[]) {
    try {
        if(!std::filesystem::is_directory((std::string)VAR_PATH + "/packages/"))
            std::filesystem::create_directories((std::string)VAR_PATH + "/packages/");
    } catch (const std::exception& e) {
        #ifdef DEBUG
        std::cout << "[DEBUG:createVar] " << e.what() << std::endl;
        #endif
        std::cout << "Unable to create package list folder. Try running pako as superuser." << std::endl;
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
