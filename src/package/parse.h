#ifndef PAKO_PARSE_H
#define PAKO_PARSE_H

#include <filesystem>
#include "package.h"

Package ParseSpecs(std::filesystem::path specFilePath);

#endif