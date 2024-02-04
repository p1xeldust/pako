#ifndef PAKO_TAR_H
#define PAKO_TAR_H

#include <filesystem>

int extractTar(std::filesystem::path source, std::filesystem::path destination);

#endif