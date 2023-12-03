#include "db/database.h"
#include "output/print.h"
#include "package/package.h"

#ifndef PAKO_H
#define PAKO_H

#ifdef __cplusplus
#include <vector>
#include <string>

int8_t installPackage(std::vector<std::string> arguments);
int8_t removePackage(std::vector<std::string> arguments);
int8_t listPackages(std::vector<std::string> arguments);

extern "C" {
#endif
    void help();
    void version();
#ifdef __cplusplus
}
#endif

#endif
