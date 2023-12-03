#ifndef PRINT_H
#define PRINT_H

#ifdef __cplusplus
extern "C" {
#endif
    void errormsg(const char* message_format, ...);
    void debugmsg(const char* format, ...);
    void msg(const char* message_format, ...);
#ifdef __cplusplus
}
#endif

#endif
