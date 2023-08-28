// Для простоты
#include <bits/stdc++.h>

extern bool getPackageInfo(std::string path, std::string packageInfo[3]);

using namespace std;
namespace fs = std::filesystem;

bool solveDeps(string packageName) {
    vector<string> dependencies;
    for(const auto entry : fs::directory_iterator((string)VAR_PATH + "/packages")) {
        if(entry.path().string() == (string)VAR_PATH + "/packages/" + packageName)
            continue;
        cout << entry.path().string() << endl;
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
        #ifdef DEBUG
            cout << "[Debug.SolveDeps] Solved dependencies!" << endl;
        #endif
        return true;
    } else {
        cout << "[i] Unable to solve dependencies. Packages";

        for(size_t i=0; i < dependencies.size(); i++)
            cout << " '" << dependencies[i] << "'";
        cout << " needs " << packageName << endl;
        cout << "Use --force-remove to avoid dependencies check." << endl;
        return false;
    }
}
void removePackageSource(string listFilePath) {
    vector<string> files;
    ifstream listFile(listFilePath);
    for(string line; getline(listFile, line); listFile.good())
        files.push_back(std::move((string)PREFIX + "/" + line));
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
