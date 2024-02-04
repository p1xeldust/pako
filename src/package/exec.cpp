#include <filesystem>
#include <unistd.h>

#include "package.h"

int execScript(std::filesystem::path installScriptPath, enum execMode mode) {
    switch (mode)
    {
    case PRE_INSTALL:
        return execlp(installScriptPath.c_str(), "--preinst");
    case POST_INSTALL:
        return execlp(installScriptPath.c_str(), "--postinst");
    case PRE_REMOVE:
        return execlp(installScriptPath.c_str(), "--preremove");
    default:
        break;
    }
    return 0;
}