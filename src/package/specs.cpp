#include "package.h"
#include "arch.h"

#include "../essential/o.h"
extern Output output;

int checkSpecs(class Package& package) {
    if (!package.name.length()) {
        output.error("specs_check.cpp: Incorrect package name. Stopping.");
        return -1;
    }

    if (!package.version.length()) {
        output.error("specs_check.cpp: Incorrect version. Stopping.");
        return -1;
    }

    if (!package.arch.name.length()) {
        output.error("specs_check.cpp: Incorrect arch. Stopping.");
        return -1;
    }
    return checkArch(package);
}