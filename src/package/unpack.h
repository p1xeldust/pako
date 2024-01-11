#ifndef PAKO_UNPACK_H
#define PAKO_UNPACK_H

#include <string>
#include "package.h"

int unpackPackage(std::string source, Package& package, std::string tmpPath);

#endif