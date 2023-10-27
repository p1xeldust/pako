#include <stdio.h>

void help() {
    printf("%s (%s)\n\n%s\n%s\n%s\n%s\n%s\n%s\n",
        "pako",
        VERSION,
        "Usage: pako <option> [arguments]",
        "    -i    install packages",
        "    -r    remove packages",
        "    -l    list installed packages",
        "    -h    print help message",
        "    -v    print version");
}
