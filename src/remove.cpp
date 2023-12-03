#include <iostream>
#include <vector>
#include <filesystem>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>

#include "pako.hpp"

using namespace std;

bool force_remove = false;

int8_t removePackage(std::vector<std::string> arguments) {
    if(geteuid() != 0) {
		errormsg("[i] Operation requires superuser privileges.");
		return 127;
	}

    for(size_t i=0; i < arguments.size(); i++) {
        if(arguments[i] == "--force-remove") {
            force_remove = true;
	        arguments.erase(arguments.begin() + i);
            continue;
        }
    }

    for(size_t i = 0; i < arguments.size(); i++) {
        if(!isInDatabase(arguments[i].c_str())) {
            errormsg(("Package " + arguments[i] + " is not installed").c_str());
            return 1;
        }
        char* packageData[5];
        readDBPackageData(arguments[i].c_str(), packageData);
        #ifdef DEBUG
        debugmsg("dataFilePath %s\n", packageData[3]);
        debugmsg("listFilePath %s\n", packageData[4]);
        #endif
        msg("Preparing %s:%s (%s)\n", packageData[0], packageData[1], packageData[2]);
        if(!force_remove)
            if(!solve_dependencies(packageData[0]))
                return 2;
        remove_package_source(packageData[4]);
        std::filesystem::remove((string)VAR_PATH + "/control/" + arguments[i] + ".files");
        std::filesystem::remove((string)VAR_PATH + "/control/" + arguments[i] + ".info");
        if(std::filesystem::exists((string)VAR_PATH + "/control/" + arguments[i] + ".install"))
            std::filesystem::remove((string)VAR_PATH + "/control/" + arguments[i] + ".install");
        removePackageFromDatabase(packageData[0]);
        msg("Removed %s:%s (%s)\n", packageData[0], packageData[1], packageData[2]);
    }
    #if EASTER_EGG == 1
	std::cout << "That's how it works!" << std::endl;
	#endif
    return 0;
}
