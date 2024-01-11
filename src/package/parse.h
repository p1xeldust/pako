#ifndef PAKO_PARSE_H
#define PAKO_PARSE_H

#include <string>
#include "package.h"

using std::string;

int parseSpecs(string specFilePath, Package& package);

#endif