#include <string.h>

#ifndef PRINT_H
#define PRINT_H

class Print {
public:
    void errormsg(std::string message);
    void debugmsg(std::string message);
    void msg(std::string message);

};

#endif
