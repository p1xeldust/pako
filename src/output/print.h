#include <string>

#ifndef PRINT_H
#define PRINT_H
class Print {
private:

public:
    void errormsg(std::string message);
    void debugmsg(std::string message);
    void msg(std::string message);

};

#endif
