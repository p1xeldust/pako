#include <cstdint>
#include <iostream>
#include <string>
#include <filesystem>
#include <variant>

using namespace std;
namespace fs = std::filesystem;
extern bool getPackageInfo(std::string path, std::string packageInfo[3]);

uint8_t List(int argc,char* argv[]) {
    string packageListPath = (string)VAR_PATH + "/packages/";
    string packageInfo[3];
    if(argc > 2) {
        cout << left << 
                setw(16) << "Name" << 
                setw(8) << "Version" << 
                setw(6) << "Arch" << 
                endl;
        for(int i=2; i<argc; i++) {
            if(getPackageInfo(packageListPath + argv[i], packageInfo)) {
                cout << left << 
                    setw(16) << packageInfo[0] << 
                    setw(8) << packageInfo[2] << 
                    setw(6) << packageInfo[1] << 
                    endl;
            } else {
                cout << left << 
                    setw(16) << argv[i] << 
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
            getPackageInfo(entry.path().string() + "/info", packageInfo);
            cout << left << 
                    setw(16) << packageInfo[0] << 
                    setw(8) << packageInfo[2] << 
                    setw(6) << packageInfo[1] << 
                    endl;
        }
    }
    return 0;
}