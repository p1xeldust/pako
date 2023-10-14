// Для простоты
#include <bits/stdc++.h>
#include <filesystem>
#include <unistd.h>

extern bool getPackageData(std::string path, std::string packageInfo[3]);
extern void debugmsg(std::string msg);

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
        cout << "[i] Unable to solve dependencies. Packages";
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
            debugmsg("[DEBUG] Removed file " + files[i]);
        }
        else if(fs::exists(files[i]) && fs::is_directory(files[i])) {
            fs::remove(files[i]);
            debugmsg("[DEBUG] Removed symlink " + files[i]);
        }
        else if(fs::exists(files[i]) && fs::is_symlink(files[i])) {
            fs::remove(files[i]);
            debugmsg("[DEBUG] Removed symlink " + files[i]);
        }
    }
}

bool flag_force_remove = false;
int8_t Remove(std::vector<std::string> arguments) {
    if(geteuid() != 0) {
		cerr << "[i] Operation requires superuser privileges." << endl;
		return 127;
	}

    for(int i = 0; i < arguments.size(); i++) {
        if(arguments[i] == (string)"--force-remove") {
            flag_force_remove = true;
            arguments.erase(std::find(arguments.begin(), arguments.end(), "--force-remove"));
            continue;
        }
        
        if(!fs::exists((string)VAR_PATH + "/packages/" + arguments[i])) {
            cout << "Package " << arguments[i] << " is not installed" << endl;
            return 1;
        }

        string dataFilePath = (string)VAR_PATH + "/packages/" + arguments[i] + "/info";
        string listFilePath = (string)VAR_PATH + "/packages/" + arguments[i] + "/files";
        
        string packageInfo[3];

        debugmsg("[Debug] dataFilePath " + dataFilePath);
        debugmsg("[Debug] listFilePath " + listFilePath);

        cout << "[i] Preparing " << arguments[i] << endl;
        if(!flag_force_remove)
            if(!solveDeps(packageInfo[0])) {
                return 2;
            }
    
        removePackageSource(listFilePath);
        fs::remove_all((string)VAR_PATH + "/packages/" + arguments[i]);
        cout << "[i] Removed " << arguments[i] << endl;
    }
        #if EASTER_EGG == 1
	std::cout << "Message from developer: Thank you!" << std::endl;
	#endif
    return 0;
}
