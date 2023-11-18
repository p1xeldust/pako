#include <iostream>
#include "../pako.hpp"

void Pako::help() {
    std::cout
     << "pako " << VERSION << "\n"
     << "Usage: pako <option> [arguments]\n"
     << "    -i    install packages\n"
     << "    -r    remove packages\n"
     << "    -l    list installed packages\n"
     << "    -h    print help message\n"
     << "    -v    print version\n\n";
}
