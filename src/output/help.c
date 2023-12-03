#include <stdio.h>
#include "../pako.hpp"

void help() {
    printf("pako %s\n%s\n%s\n%s\n%s\n%s\n%s\n\n",
            VERSION,
            "Usage: pako <option> [arguments]",
            "    -i    install packages",
            "    -r    remove packages",
            "    -l    list installed packages",
            "    -h    print help message",
            "    -v    print version");
}
