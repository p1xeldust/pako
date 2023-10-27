#include <stdio.h>

void debugmsg(const char* message) {
    #ifdef DEBUG
    printf("%s%s%s\n", "\e[1;90m[DEBUG] ", message, "\e[1;0m");
    #endif
}

void msg(const char* message) {
    printf("%s%s%s\n", "\e[52;1m", message, "\e[1;0m");
}
void errormsg(const char* message) {
    printf("%s%s%s\n", "\e[1;31mError\e[0;1m\e[5;1m: ", message, "\e[1;0m");
}
