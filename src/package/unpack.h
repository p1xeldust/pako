#ifndef PAKO_UNPACK_H
#define PAKO_UNPACK_H

#include <filesystem>
#include "package.h"

int unpackPackage(std::filesystem::path packageFile);

#endif