#ifndef PAKO_COPY_H
#define PAKO_COPY_H

#include <filesystem>
#include <fstream>
#include "../package/package.h"

void copyByList(const std::filesystem::path& tmpSourcePath, std::ifstream &listFile, Package &package);

#endif