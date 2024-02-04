#include <sys/utsname.h>
#include <string>

#include "../common/dialog.h"
#include "package.h"

#include "architecture.h"
#include "dependencies.h"

Package CheckArchitecture(Package &package)
{
    struct utsname hostInfo;
    uname(&hostInfo);

    if (package.arch.name == "none") {
        package.arch.meta = ARCH_NONE;
    }
    else if (((std::string)hostInfo.machine == package.arch.name))  
        package.arch.meta = ARCH_NATIVE;
    else {
        package.arch.meta = ARCH_FOREIGN;
        package.skipcurrent = dialog.SolveArchitecture(package, (std::string)hostInfo.machine);
    }
    return CheckDependencies(package);
}