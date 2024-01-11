#ifndef PAKO_COPY_LIST_H
#define PAKO_COPY_LIST_H

#include <fstream>
#include <string>

#include "package.h"

void copyByList(std::string tmpSourcePath, std::ifstream& listFile, Package& package);

#endif