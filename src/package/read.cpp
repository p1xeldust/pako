#include <fstream>

#include "package.h"

bool Package::read_package_data(std::string dataFilePath, std::string packageData[3]) {
    std::ifstream dataFile(dataFilePath);
    for(std::string line, type, value; std::getline(dataFile, line); dataFile.good()) {
        type = line.substr(0, line.find_first_of(": "));
        value = line.substr(line.find_last_of(" ")+1);
        if(type == "name")
            packageData[0] = value;
        else if(type == "version")
            packageData[2] = value;
        else if(type == "arch")
            packageData[1] = value;
        }
        #ifdef DEBUG_GPI
        for(uint8_t i = 0; i<3; i++) {
                if(packageData[i].size() <= 0)
                        return 0;
               out.debugmsg("DEBUG_GPI: PackageData: " + packageData[i]);
        }
        #endif
        dataFile.close();
        return 1;
}
