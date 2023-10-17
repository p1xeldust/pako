#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <algorithm>

using namespace std;
namespace fs = std::filesystem;
extern bool getPackageData(std::string path, std::string packageInfo[3]);
extern void debugmsg(std::string msg);

void printHeader() {
    cout << left
	 << setw(46) << setfill('_') << " " << setfill(' ') << " " << endl
         << "| " << setw(24) << "Name"
         << "| " << setw(8)  << "Version"
         << "| " << setw(8)  << "Arch"
         << "| " << endl << left
         << setw(26) << setfill('_') << "|" << setw(10) << "|" << setw(10) << "|" << setfill(' ') << "|" << endl;
}

void printFooter() {
    cout << left
	 << setw(26) << setfill('_') << "|" << setw(10) << "|" << setw(10) << "|" << setfill(' ') << "|" << endl
         << "| " << setw(44) << "pako " + (string)VERSION << "|" << endl
	<< setw(46) << setfill('_') << "|" << setfill(' ') << "|" << endl;
}

int8_t List(std::vector<std::string> arguments) {
    string packageListPath = (string)VAR_PATH + "/packages/";
    string packageInfo[3];
    if(arguments.size() > 0) {
	    printHeader();
            for(size_t i=0; i<arguments.size(); i++) {
                if(fs::exists(packageListPath + arguments[i]) && fs::is_directory(packageListPath + arguments[i])) {
		    debugmsg("Found package in " + arguments[i]);
                    getPackageData(packageListPath + arguments[i] + "/info", packageInfo);
                    cout << left
                         << "| " << setw(24) << packageInfo[0]
                         << "| " << setw(8)  << packageInfo[2]
                         << "| " << setw(8)  << packageInfo[1]
                         << "|"  << endl;
                } else {
                    debugmsg("Not found package in " + arguments[i]);
                    cout << left
                         << "| " << setw(24) << arguments[i]
                         << "| " << setw(8)  << "unknown"
                         << "| " << setw(8)  << "unknown"
                         << "|"  << endl;
	        }
            }
       printFooter();
    } else {
	printHeader();
        for(const auto& entry : fs::directory_iterator(packageListPath)) {
            getPackageData(entry.path().string() + "/info", packageInfo);
            cout << left
                 << "| " << setw(24) << packageInfo[0]
                 << "| " << setw(8)  << packageInfo[2]
                 << "| " << setw(8)  << packageInfo[1]
                 << "|" << endl;
	}
        printFooter();
    }
    return 0;
}
