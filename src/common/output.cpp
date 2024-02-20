#include <iostream>
#include "output.h"



void Output::msg(const char* message) {
    std::cout << message << "\033[0m\n";
}

void Output::error(const char* message) {
    std::cout << "\x1b[91m\x1b[1merror:\033[0m " << message << "\033[0m\n";
}

/*
void Output::debug(const char* message) {
    cout << "\033[1;90m[DEBUG] " << message << "\033[0m\n";
}
*/

void Output::warn(const char* message) {
    std::cout << "\x1b[93m\x1b[1mwarn:\033[0m " << message << "\033[0m\n";
}

void Version() {
    std::cout << "pako " << VERSION << "\n";
}

void Help() {
    std::cout << "\npako " << VERSION << "\n"
              << "Usage: pako <option> [arguments]\n"
              << "    -i    install packages\n"
              << "    -r    remove packages\n"
              << "    -l    list installed packages\n"
              << "    -h    print help message\n"
              << "    -v    print version\n\n";

}