/*
    Architecture check via uname (utsname)
    Written by Paul Goldstein, Jan 2024

*/

#include <sys/utsname.h>

#include "dependencies.h"
#include "package.h"

#include "../essential/dialog.h"
#include "../essential/o.h"
extern Output output;
extern Dialog dialog;

string getHostArch() {
    struct utsname hostSysInfo;
    uname(&hostSysInfo);
    return (string)hostSysInfo.machine;
}

int checkArch(Package& package) {
    string hostArch = getHostArch();
    if (!hostArch.length()) {
        output.error("Incorrect architecture, broken package.");
        return -1;
    }

    if (hostArch == "none")
        package.arch.meta = ARCH_NONE;
    else if (hostArch == package.arch.name)
        package.arch.meta = ARCH_NATIVE;
    else {
        package.arch.meta = ARCH_FOREIGN;
        dialog.solve_arch(package);
    }
    return CheckDeps(package);
}
