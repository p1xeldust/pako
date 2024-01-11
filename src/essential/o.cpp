#include <iostream>
#include <string>

#include "o.h"

using std::string, std::cout;

Output output;

void Output::msg(string message) {
    cout << message << "\033[0m\n";
}

void Output::error(string message) {
    cout << "[\033[1;31mError\033[0m] " << message << "\033[0m\n";
}

void Output::debug(string message) {
    cout << "\033[1;90m[DEBUG] " << message << "\033[0m\n";
}

void Output::warn(string message) {
    cout << "[\033[1;33mWarn\033[0m] " << message << "\033[0m\n";
}