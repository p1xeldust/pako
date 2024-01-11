#pragma once 

#ifndef OUTPUT_H
#define OUTPUT_H

#include <string>

class Output {
public:
    void msg(std::string message);
    void error(std::string message);
    void debug(std::string message);
    void warn(std::string message);
};

#endif
