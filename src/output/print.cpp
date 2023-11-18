#include <iostream>

#include "print.h"

void Print::debugmsg(std::string message) {
    #ifdef DEBUG
    std::cout << "\e[1;90m[DEBUG] " << message << "\e[1;0m\n";
    #endif
}

void Print::msg(std::string message) {
    std::cout << "\e[52;1m" << message << "\e[1;0m\n";
}
void Print::errormsg(std::string message) {
    std::cout << "\e[1;31mError\e[0;1m: " << message << "\e[1;0m\n";
}
