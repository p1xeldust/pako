#ifndef PAKO_EXEC_H
#define PAKO_EXEC_H

#include <filesystem>

#include "package.h"

int execScript(std::filesystem::path installScriptPath, enum execMode mode);

#endif