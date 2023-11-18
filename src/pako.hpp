#include <vector>
#include <string>
#include "db/database.h"
#include "output/print.h"
#include "package/package.h"

#ifndef PAKO_H
#define PAKO_H

class Pako {
private:
    Package pkg;
    Print out;
    Database db;
public:
    int8_t install(std::vector<std::string> arguments);
    int8_t remove(std::vector<std::string> arguments);
    int8_t list(std::vector<std::string> arguments);
    void help();
    void version();
};

#endif
