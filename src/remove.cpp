#include <asm-generic/errno.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <unistd.h>
#include <vector>

extern bool getPackageInfo(std::string path, std::string packageInfo[3]);

using namespace std;
namespace fs = std::filesystem;

bool solveDeps(string packageName) {
    vector<string> dependencies;
    #ifdef DEBUG
        cout << "Solver active" << endl;
    #endif
    
    for(const auto entry : fs::directory_iterator((string) VAR_PATH + "/packages")) {   
        ifstream dataFile(entry.path().string() + "/info", ios::in);

	    for(string line; getline(dataFile, line); dataFile.good()) {

    	    if(line.find(':') != std::string::npos) {
	    		string type = line.substr(0, line.find(':'));
    			string value = line.substr(line.find(':') + 1);

			    type.erase(type.find_last_not_of(" \t") + 1);
		     	value.erase(0, value.find_first_not_of(" \t"));

	    		if(type == "deps" && value == packageName) {
       			    #ifdef DEBUG
                        cout << "[Debug:checkDeps] Dependency found: '" << entry.path().string().erase(0, entry.path().string().find_last_of("/")+1) << "'" << endl;
               	    #endif
				    dependencies.push_back(std::move(entry.path().string().erase(0, entry.path().string().find_last_of("/")+1)));
                    break;
    		    } else
			    	continue;
		    }
	    }
    }
	if(dependencies.size() <= 0) {
        #ifdef DEBUG
            cout << "[Debug.SolveDeps] Solved dependencies!" << endl;
        #endif
        return true;
    } else {
        cout << "[i] Unable to solve dependencies. Packages";

        for(size_t i=0; i < dependencies.size(); i++)
            cout << " " << dependencies[i];
        cout << " require " << packageName << endl;
        cout << "Use --force-remove to avoid dependencies check." << endl;
        return false;
    }
}
void removePackageSource(string listFilePath) {
    vector<string> files;
    ifstream listFile(listFilePath);
    for(string line; getline(listFile, line); listFile.good())
        files.push_back(std::move((string) PREFIX + "/" + line));
    for(size_t i; i<files.size(); i++)
        if(fs::exists(files[i]) && fs::is_regular_file(files[i]))
            fs::remove(files[i]);
        
}
// Самая простая функция (должна быть)



uint8_t Remove(int argc, char* argv[]) {
    if(geteuid() != 0) {
		cerr << "[i] Action requires superuser privileges." << endl;
		return 127;
	}
    bool flag_force_remove = false;
    for(int i=2; i<argc; i++)
    
        if((string)"--force-remove" == argv[i]) {
            flag_force_remove = true;
            break;
        }
    
    for(int i = 2; i < argc; i++) {
        if((string)"--force-remove" == argv[i])
            continue;
        string dataFilePath = (string)VAR_PATH + "/packages/" + argv[i] + "/info";
        string listFilePath = (string)VAR_PATH + "/packages/" + argv[i] + "/files";
        string packageInfo[3];
    #ifdef DEBUG
        cout << "[Debug] dataFilePath" << dataFilePath << endl;
        cout << "[Debug] listFilePath" << listFilePath << endl;
    #endif
	
    if(!getPackageInfo(dataFilePath, packageInfo)) {
        cout << "Package is not installed"  << endl;
        return 1;
    };
    cout << "[i] Preparing " << argv[i] << endl;
    if(!flag_force_remove)
        if(!solveDeps(packageInfo[0])) {
            cout << "Package is not installed"  << endl;
            return 2;
        }
    
    removePackageSource(listFilePath);
    fs::remove_all((string)VAR_PATH + "/packages/" + argv[i]);
    cout << "[i] Removed " << argv[i] << endl;
    }
    #if EASTER_EGG == 1 
		std::cout << "Message from developer: Thank you!" << std::endl;
	#endif
    return 0;
}
