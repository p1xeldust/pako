#include <unistd.h>

#include "package.h"

int execScript(string installScript, int mode) {
    switch (mode)
    {
    case PRE_INSTALL:
        return execlp(installScript.c_str(), "preinstall");
    case POST_INSTALL:
        return execlp(installScript.c_str(), "postinstall");
    case PRE_REMOVE:
        return execlp(installScript.c_str(), "preremove");
    case POST_REMOVE:
        return execlp(installScript.c_str(), "postremove");
    default:
        break;
    }
    return 0;
}