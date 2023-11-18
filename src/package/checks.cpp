#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sys/utsname.h>
#include <filesystem>
#include <string>
#include <sstream>

#include "package.h"

using std::string, std::to_string, std::ifstream, std::ios, std::vector, std::istringstream;

bool Package::checkArch(std::string packageArch) {
        struct utsname hostData;
        uname(&hostData);
        out.debugmsg("checkArch: Host Arch: " + (std::string)hostData.machine);
        out.debugmsg("checkArch: Package Arch: " + packageArch);
        if(hostData.machine != packageArch)
                return false;
        return true;
}

bool Package::checkDeps(string dataFilePath) {
    vector<string> dependencies;
    string packageData[3];
    readPackageData(dataFilePath, packageData);
    ifstream dataFile(dataFilePath, ios::in);   
    for(string line; getline(dataFile, line); dataFile.good())
        if(line.find("deps:") != string::npos) {
            istringstream dependenciesList(line.substr(line.find(':') + 1).erase(0, line.find_first_not_of(" \t")));
            for(string dep; dependenciesList >> dep;)
                if(dep.size() > 0) {
                    out.debugmsg("checkDeps: Dependency found for " + packageData[0] + " '" + dep + "'");
                    dependencies.push_back(dep);
                } else break;
        }
        /*  Проверка на установленные пакеты */
    if(dependencies.size() > 0) {
        for(const auto dep : dependencies) {
	        /* Проверка среди установленных */
            if(db.isInDatabase(dep)) {
                out.debugmsg("checkDeps: Dependency solved for " + packageData[0] + ": " + dep);
                dependencies.erase(std::remove(dependencies.begin(), dependencies.end(), dep), dependencies.end());
            }
        /* Проверка среди устанавливаемых */
        for(const auto dep : dependencies) {
            for(const auto& entry : std::filesystem::directory_iterator((std::string)TMP_PATH)) {
                std::string depPackageData[3];
                readPackageData(entry.path().string() + "/package/PAKO/info", depPackageData);
                    if(dep == depPackageData[0]) {
                        out.debugmsg("checkDeps: Dependency solved for " + packageData[0] + ": " + dep);
                        dependencies.erase(std::remove(dependencies.begin(), dependencies.end(), dep), dependencies.end());
                        }
                }
            }
        }
        }
    // Если пакеты остались в списке зависимостей, не установленные в системе или не готовые для установки, то пожаловаться на это.
	if(dependencies.size() > 0) {
        out.errormsg("Dependent packages found:");
		readPackageData(dataFilePath, packageData);
		for(const auto dep : dependencies)
		    out.msg(" (*) Package " + packageData[0] + " depends on: " + dep);
		out.msg("Install these packages first.");
		return false;
	}
	return true;
}
bool Package::checkConflicts(string dataFilePath) {
	vector<string > conflicts;
	string packageData[3];
	ifstream dataFile(dataFilePath, ios::in);
	for(string  line; getline(dataFile, line); dataFile.good()) {
		if(line.find("conflicts:") != string::npos) {
			istringstream conflictsList(line.substr(line.find(':') + 1).erase(0, line.find_first_not_of(" \t")));
			for(string  conflict; conflictsList >> conflict;) {
                if(db.isInDatabase(conflict)) {
                    conflicts.push_back(conflict);
                    continue;
                }
                for(const auto& entry : std::filesystem::directory_iterator((string)TMP_PATH)) {
                    readPackageData(entry.path().string() + "/package/PAKO/info", packageData);
                    if(conflict == packageData[0])
                        conflicts.push_back(conflict);
                }
            }
        }
    }
    out.debugmsg(to_string(conflicts.size()));
	if(conflicts.size() > 0) {
        readPackageData(dataFilePath, packageData);
            out.errormsg("Conflicting packages found:");
		for(const auto conf : conflicts)
			out.msg(" (*) Package " + packageData[0] + " conflicts with " + conf);
		return false;
	}
	return true;
}
