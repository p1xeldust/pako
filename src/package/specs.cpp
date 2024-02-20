
#include "../common/output.h"
#include "architecture.h"
#include "package.h"

Package checkSpecs(class Package& package) {
    if (package.name.empty()) {
        output.error("specs.cpp: Incorrect package name.");
        package.skipcurrent = 1;
        return package;
    }

    if (package.version.empty()) {
        output.error("specs.cpp: Incorrect version.");
        package.skipcurrent = 1;
        return package;
    }

    if (package.arch.name.empty()) {
        output.error("specs_check.cpp: Incorrect arch.");
        package.skipcurrent = 1;
        return package;
    }
    return CheckArchitecture(package);
}
