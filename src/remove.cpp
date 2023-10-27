#include <iostream>
#include <vector>
#include <filesystem>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>

extern bool getPackageData(std::string dataFilePath, std::string packageData[3]);

extern "C" void debugmsg(const char* message);
extern "C" void msg(const char* message);
extern "C" void errormsg(const char* message);

using namespace std;
namespace fs = std::filesystem;

bool solveDeps(string packageName) {
    vector<string> dependencies;
    for(const auto entry : fs::directory_iterator((string)VAR_PATH + "/packages")) {
        if(entry.path().string() == (string)VAR_PATH + "/packages/" + packageName)
            continue;
        ifstream dataFile(entry.path().string() + "/info", ios::in);
    
        for(string line; getline(dataFile, line); dataFile.good()) {

            if(line.find("deps:") != std::string::npos) {
                istringstream dependenciesList(line.substr(line.find(':') + 1).erase(0, line.find_first_not_of(" \t")));
                string dependency;
                for(string dependency; dependenciesList >> dependency;)
                    if(dependency == packageName)
                        dependencies.push_back(std::move(entry.path().string().substr(entry.path().string().find_last_of("/") +1)));
		    }
	    }
        
    }
    if(dependencies.size() <= 0) {
        debugmsg("[Debug.SolveDeps] Solved dependencies!");
        return true;
    } else {
        msg("[i] Unable to solve dependencies. These packages:");
        for(size_t i=0; i < dependencies.size(); i++)
            cout << " '" << dependencies[i] << "'";
        cout << " depends on " << packageName
             << endl
             << "[i] Use --force-remove to avoid this check."
             << endl;
        return false;
    }
}
void removePackageSource(string listFilePath) {
    vector<string> files;
    ifstream listFile(listFilePath);
    for(string line; getline(listFile, line); listFile.good()) {
        files.push_back(std::move((string)PREFIX + "/" + line));
        //debugmsg("[DEBUG] Added " + (string)PREFIX + "/" + line + " to remove list");
    }
    /* Удаление файлов */
    for(size_t i=0; i<files.size(); i++) {
        if(fs::exists(files[i]) && !fs::is_symlink(files[i])) {
            fs::remove(files[i]);
            debugmsg(("[DEBUG] Removed file " + files[i]).c_str());
        }
    }
    for(size_t i=0; i<files.size(); i++) {
        if(fs::exists(files[i]) && fs::is_directory(files[i])) {
            fs::remove(files[i]);
            debugmsg(("[DEBUG] Removed directory " + files[i]).c_str());
        }
    }
    for(size_t i=0; i<files.size(); i++) {
        if(fs::exists(files[i]) && fs::is_symlink(files[i])) {
            fs::remove(files[i]);
            debugmsg(("[DEBUG] Removed symlink " + files[i]).c_str());
        }
    }
}

bool force_remove = false;
int8_t Remove(std::vector<std::string> arguments) {
    if(geteuid() != 0) {
		errormsg("[i] Operation requires superuser privileges.");
		return 127;
	}

    for(int i = 0; i < arguments.size(); i++) {
        //debugmsg(arguments[i]);
        for(size_t j=0; j<arguments.size(); j++) {
            if(arguments[j] == "--force-remove") {
                force_remove = true;
	        arguments.erase(std::remove(arguments.begin(), arguments.end(), "--force-remove"), arguments.end());
                continue;
            }
        }

        if(!fs::exists((string)VAR_PATH + "/packages/" + arguments[i])) {
            errormsg(("Package " + arguments[i] + " is not installed").c_str());
            return 1;
        }

        string dataFilePath = (string)VAR_PATH + "/packages/" + arguments[i] + "/info";
        string listFilePath = (string)VAR_PATH + "/packages/" + arguments[i] + "/files";
        string packageData[3];

        debugmsg(("[Debug] dataFilePath " + dataFilePath).c_str());
        debugmsg(("[Debug] listFilePath " + listFilePath).c_str());
	    getPackageData(dataFilePath, packageData);
        msg(("Preparing " + packageData[0] + ":" + packageData[1] + " (" + packageData[2] + ")").c_str());
        if(!force_remove) {
            if(!solveDeps(packageData[0])) {
                return 2;
            }
        }
        removePackageSource(listFilePath);
        fs::remove_all((string)VAR_PATH + "/packages/" + arguments[i]);
        msg(("Removed " + arguments[i]).c_str());
    }
        #if EASTER_EGG == 1
	std::cout << "That's how it works!" << std::endl;
	#endif
    return 0;
}
