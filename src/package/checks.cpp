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

bool Package::check_architecture(std::string packageArch) {
        struct utsname host_system_information;
        uname(&host_system_information);
        out.debugmsg("checkArch: Host Arch: " + (std::string)host_system_information.machine);
        out.debugmsg("checkArch: Package Arch: " + packageArch);
        if(packageArch == "amd64") {
            out.debugmsg("checkArch: Oh, seems like package have amd64 instead x86_64. Processing anyway!");
            packageArch = "x86_64";
        }
        if(host_system_information.machine != packageArch)
            return false;
        return true;
}

bool Package::check_dependencies(string package_data_file_path) {
    vector<string> dependencies;
    string package_data[3];
    read_package_data(package_data_file_path, package_data);
    ifstream dataFile(package_data_file_path, ios::in);   
    for(string line; getline(dataFile, line); dataFile.good())
        if(line.find("deps:") != string::npos) {
            istringstream dependenciesList(line.substr(line.find(':') + 1).erase(0, line.find_first_not_of(" \t")));
            for(string dependency; dependenciesList >> dependency;)
                if(dependency.size() > 0) {
                    out.debugmsg("checkDeps: Dependency found for " + package_data[0] + " '" + dependency + "'");
                    dependencies.push_back(dependency);
                } else break;
        }
        /*  Проверка на установленные пакеты */
    if(dependencies.size() > 0) {
        for(const auto dependency : dependencies) {
	        /* Проверка среди установленных */
            if(db.isInDatabase(dependency)) {
                out.debugmsg("checkdependencys: Dependency solved for " + package_data[0] + ": " + dependency);
                dependencies.erase(std::remove(dependencies.begin(), dependencies.end(), dependency), dependencies.end());
            }
        /* Проверка среди устанавливаемых */
        for(const auto dependency : dependencies) {
            for(const auto& entry : std::filesystem::directory_iterator((std::string)TMP_PATH)) {
                std::string dep_package_data[3];
                read_package_data(entry.path().string() + "/package/PAKO/info", dep_package_data);
                    if(dependency == dep_package_data[0]) {
                        out.debugmsg("checkDeps: Dependency solved for " + package_data[0] + ": " + dependency);
                        dependencies.erase(std::remove(dependencies.begin(), dependencies.end(), dependency), dependencies.end());
                        }
                }
            }
        }
        }
    // Если пакеты остались в списке зависимостей, не установленные в системе или не готовые для установки, то пожаловаться на это.
	if(dependencies.size() > 0) {
        out.errormsg("Dependent packages found:");
		read_package_data(package_data_file_path, package_data);
		for(const auto dependency : dependencies)
		    out.msg(" (*) Package " + package_data[0] + " depends on: " + dependency);
		out.msg("Install these packages first.");
		return false;
	}
	return true;
}
bool Package::check_conflicts(string dataFilePath) {
	vector<string > conflicts;
	string package_data[3];
	ifstream dataFile(dataFilePath, ios::in);
	for(string  line; getline(dataFile, line); dataFile.good()) {
		if(line.find("conflicts:") != string::npos) {
			istringstream conflictsList(line.substr(line.find(':') + 1).erase(0, line.find_first_not_of(" \t")));
			for(string conflict; conflictsList >> conflict;) {
                if(db.isInDatabase(conflict)) {
                    conflicts.push_back(conflict);
                    continue;
                }
                for(const auto& entry : std::filesystem::directory_iterator((string)TMP_PATH)) {
                    read_package_data(entry.path().string() + "/package/PAKO/info", package_data);
                    if(conflict == package_data[0])
                        conflicts.push_back(conflict);
                }
            }
        }
    }
    out.debugmsg(to_string(conflicts.size()));
	if(conflicts.size() > 0) {
        read_package_data(dataFilePath, package_data);
            out.errormsg("Conflicting packages found:");
		for(const auto conf : conflicts)
			out.msg(" (*) Package " + package_data[0] + " conflicts with " + conf);
		return false;
	}
	return true;
}
