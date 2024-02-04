#ifndef PAKO_OUTPUT_H
#define PAKO_OUTPUT_H

#include <string>

class Output
{
public:
    void msg(const char *message);
    void warn(const char *message);
    void error(const char *message);

    void msg(std::string message)
    {
        msg(message.c_str());
    }
    void warn(std::string message)
    {
        warn(message.c_str());
    }
    void error(std::string message)
    {
        error(message.c_str());
    }
};

extern Output output;

#endif