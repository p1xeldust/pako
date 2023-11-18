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

int8_t Pako::remove(std::vector<std::string> arguments) {
    if(geteuid() != 0) {
		out.errormsg("[i] Operation requires superuser privileges.");
		return 127;
	}

    for(size_t i=0; i < arguments.size(); i++) {
        for(size_t j=0; j<arguments.size(); j++) {
            if(arguments[j] == "--force-remove") {
                force_remove = true;
	            arguments.erase(arguments.begin() + j);
                continue;
            }
        }
    }

    for(int i = 0; i < arguments.size(); i++) {
        if(!db.isInDatabase(arguments[i])) {
            out.errormsg("Package " + arguments[i] + " is not installed");
            return 1;
        }
        string* packageData = db.readDBPackageData(arguments[i]);

        out.debugmsg("dataFilePath " + packageData[3]);
        out.debugmsg("listFilePath " + packageData[4]);
        out.msg("Preparing " + packageData[0] + ":" + packageData[1] + " (" + packageData[2] + ")");
        if(!force_remove)
            if(!pkg.solveDeps(packageData[0]))
                return 2;
        pkg.removePackageSource(packageData[4]);
        std::filesystem::remove((string)VAR_PATH + "/control/" + arguments[i] + ".files");
        std::filesystem::remove((string)VAR_PATH + "/control/" + arguments[i] + ".info");
        if(std::filesystem::exists((string)VAR_PATH + "/control/" + arguments[i] + ".install"))
            std::filesystem::remove((string)VAR_PATH + "/control/" + arguments[i] + ".install");
        db.removePackage(packageData[0]);
        out.msg("Removed " + arguments[i]);
    }
    #if EASTER_EGG == 1
	std::cout << "That's how it works!" << std::endl;
	#endif
    return 0;
}
