#include <stdio.h>
#include <stdarg.h>

#include "print.h"

void debugmsg(const char* format, ...) {
    #ifdef DEBUG
    va_list args;
    va_start(args, format);
    printf("\e[1;90m[DEBUG] ");
    vfprintf(stdout, format, args);
    printf("\033[0;0m");
    va_end(args);
    #endif
}

void msg(const char* message_format, ...) {
    va_list args;
    va_start(args, message_format);
    printf("\033[0;1m");
    vfprintf(stdout, message_format, args);
    printf("\033[0;0m");
    va_end(args);
}

void errormsg(const char* message_format, ...) {
    va_list args;
    va_start(args, message_format);
    printf("\033[1;31mError\033[0;1m: ");
    vfprintf(stdout, message_format, args);
    printf("\033[0;0m");
    va_end(args);
}