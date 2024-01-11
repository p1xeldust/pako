#ifndef PAKO_DIALOG_H
#define PAKO_DIALOG_H

#include <string>
#include "../package/package.h"

class Dialog {
public:
    int solve_copy(std::string oldFilePath, Package& package);
    int solve_arch(Package& package);
};

#endif