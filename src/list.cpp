// Для простоты
#include <bits/stdc++.h>

using namespace std;
namespace fs = std::filesystem;
extern bool getPackageData(std::string path, std::string packageInfo[3]);

int8_t List(std::vector<std::string> arguments) {
    string packageListPath = (string)VAR_PATH + "/packages/";
    string packageInfo[3];
    if(arguments.size() > 1) {
        cout << left << 
                setw(16) << "Name" << 
                setw(8) << "Version" << 
                setw(6) << "Arch" << 
                endl;
        for(size_t i=0; i<arguments.size(); i++) {
            if(getPackageData(packageListPath + arguments[i], packageInfo)) {
                cout << left << 
                    setw(16) << packageInfo[0] << 
                    setw(8) << packageInfo[2] << 
                    setw(6) << packageInfo[1] << 
                    endl;
            } else {
                cout << left << 
                    setw(16) << arguments[i] << 
                    setw(8) << 
                    setw(6) << 
                    endl;
            }

        }

    } else {
        cout << left << 
                setw(16) << "Name" << 
                setw(8) << "Version" << 
                setw(6) << "Arch" << 
                endl;

        for(const auto& entry  : fs::directory_iterator(packageListPath)) {
            getPackageData(entry.path().string() + "/info", packageInfo);
            cout << left << 
                    setw(16) << packageInfo[0] << 
                    setw(8) << packageInfo[2] << 
                    setw(6) << packageInfo[1] << 
                    endl;
        }
    }
    return 0;
}