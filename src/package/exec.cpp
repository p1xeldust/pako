#include <filesystem>
#include <unistd.h>

#include "package.h"

int execScript(std::filesystem::path installScriptPath, enum execMode mode) {
    switch (mode)
    {
    case PRE_INSTALL:
        return execlp(("/usr/bin/env sh " + installScriptPath.string()).c_str(), "--preinst");
    case POST_INSTALL:
        return execlp(("/usr/bin/env sh " + installScriptPath.string()).c_str(), "--postinst");
    case PRE_REMOVE:
        return execlp(("/usr/bin/env sh " + installScriptPath.string()).c_str(), "--preremove");
    case POST_REMOVE:
        return execlp(("/usr/bin/env sh " + installScriptPath.string()).c_str(), "--postremove");
    default:
        break;
    }
    return 0;
}