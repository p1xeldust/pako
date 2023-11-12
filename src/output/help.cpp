#include <iostream>

void help() {
    std::cout
     << "pako " << VERSION << std::endl
     << "Usage: pako <option> [arguments]" << std::endl
     << "    -i    install packages" << std::endl
     << "    -r    remove packages" << std::endl
     << "    -l    list installed packages" << std::endl
     << "    -h    print help message" << std::endl
     << "    -v    print version" << std::endl
     << std::endl;
}
