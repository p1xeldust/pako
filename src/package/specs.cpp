
#include "../common/output.h"
#include "architecture.h"
#include "package.h"

Package checkSpecs(class Package& package) {
    if (!package.name.length()) {
        output.error("specs_check.cpp: Incorrect package name, skipping.");
        package.skipcurrent = 1;
        return package;
    }

    if (!package.version.length()) {
        output.error("specs_check.cpp: Incorrect version, skipping.");
        package.skipcurrent = 1;
        return package;
    }

    if (!package.arch.name.length()) {
        output.error("specs_check.cpp: Incorrect arch, skipping.");
        package.skipcurrent = 1;
        return package;
    }
    return CheckArchitecture(package);
}