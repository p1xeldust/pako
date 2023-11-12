#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sys/utsname.h>
#include <filesystem>
#include <string>
#include <sstream>

#include "package.h"

bool Package::checkArch(std::string packageArch) {
        struct utsname hostData;
        uname(&hostData);
        out.debugmsg("checkArch: Host Arch: " + (std::string)hostData.machine);
        out.debugmsg("checkArch: Package Arch: " + packageArch);
        if(hostData.machine != packageArch)
                return false;
        return true;
}

bool Package::checkDeps(std::string dataFilePath) {

	std::vector<std::string> dependencies;
	std::string  packageData[3];
	std::ifstream dataFile(dataFilePath, std::ios::in);
	for(std::string  line; getline(dataFile, line); dataFile.good())
		if(line.find("deps:") != std::string::npos) {
			std::istringstream dependenciesList(line.substr(line.find(':') + 1).erase(0, line.find_first_not_of(" \t")));
			for(std::string  dependency; dependenciesList >> dependency;)
				if(dependency.size() > 0) {
					out.debugmsg("checkDeps: Dependency found for " + packageData[0] + "'" + dependency + "'");
					dependencies.push_back(std::move(dependency));
				}
		}
    /* Проверка на установленные пакеты */
	if(dependencies.size() > 0) {
		/* Проверка среди установленных */
		for(size_t i=0; i<dependencies.size(); ++i) {
				if(!db.isInDatabase(dependencies[i])) {
					out.debugmsg("checkDeps: Dependency solved for " + packageData[0] + " '" + dependencies[i] + "'");
					dependencies.erase(std::remove(dependencies.begin(), dependencies.end(), packageData[0]), dependencies.end());
					continue;
				}
		}
		for(const auto& entry : std::filesystem::directory_iterator((std::string )TMP_PATH)) {
		/* Проверка среди устанавливаемых */
        	readPackageData(entry.path().string() + "/package/PAKO/info", packageData);
			for(size_t i=0; i<dependencies.size(); ++i)
				if(dependencies[i] == packageData[0]) {
			    	out.debugmsg("checkDeps: Dependency solved: '" + dependencies[i] + "'");
		            dependencies.erase(std::remove(dependencies.begin(), dependencies.end(), packageData[0]), dependencies.end());
					continue;
				}
		}
    }
    // Если пакеты остались в списке зависимостей, не установленные в системе или не готовые для установки, то пожаловаться на это.
	if(dependencies.size() > 0) {
		out.msg("Package " + packageData[0] + " depends on:");
		for(size_t i = 0; i < dependencies.size(); i++)
		    std::cout << " \e[5;1m'" + dependencies[i] + "'\e[1;0m";
		std::cout << std::endl;
		out.msg("Install these packages first.");
		return false;
	}
	return true;
}
bool Package::checkConflicts(std::string dataFilePath) {
	std::vector<std::string > conflicts;
	std::string  packageData[3];
	std::ifstream dataFile(dataFilePath, std::ios::in);
	readPackageData(dataFilePath, packageData);
	for(std::string  line; getline(dataFile, line); dataFile.good()) {
		if(line.find("conflicts:") != std::string::npos) {
			std::istringstream conflictsList(line.substr(line.find(':') + 1).erase(0, line.find_first_not_of(" \t")));
			for(std::string  conflict; conflictsList >> conflict;) {
				for(const auto& entry : std::filesystem::directory_iterator((std::string )TMP_PATH)) {
					readPackageData(entry.path().string () + "/package/PAKO/info", packageData);
					if(conflict == packageData[0])
						conflicts.push_back(std::move(conflict));
				}
				for(const auto& entry : std::filesystem::directory_iterator((std::string )VAR_PATH + "/packages/")) {
					readPackageData(entry.path().string() + ".info", packageData);
					if(conflict == packageData[0])
						conflicts.push_back(packageData[0]);
				}
			}
		}
	}
	if(conflicts.size() > 0) {
		readPackageData(dataFilePath, packageData);
		out.msg(("Package " + packageData[0] + " conflicts with").c_str());
		for(size_t i=0; i<conflicts.size(); i++)
			std::cout << " \e[5;1m'" << conflicts[i] << "'\e[1;0m";
		std::cout << std::endl;
		return false;
	}
	return true;
}
