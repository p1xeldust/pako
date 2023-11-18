#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <algorithm>

#include "pako.hpp"

using namespace std;

int8_t Pako::list(std::vector<std::string> arguments) {
    cout << "Name" << setw(20) << "" << "arch" << setw(6) << "" << "Version" << endl << endl;
    if(arguments.size() > 0) {
            for(size_t i=0; i<arguments.size(); i++) {
                if(db.isInDatabase(arguments[i])) {

		    out.debugmsg("Found package in " + arguments[i]);
                    string* packageInfo = db.readDBPackageData(arguments[i]);
                    cout << left
                         << packageInfo[0] << setw(24-packageInfo[0].length()) << ""
                         << packageInfo[1] << setw(10-packageInfo[1].length()) << ""
                         << packageInfo[2] << setw(10-packageInfo[2].length()) << ""
                         << endl;
                } else {
                    out.debugmsg("Not found package in " + arguments[i]);
                    cout << left
                         << arguments[i] << setw(24-arguments[i].length()) << ""
                         << "unknown" << setw(3) << ""
                         << "unknown" << setw(3) << ""
                         << endl;
                 }
            }
    } else {
        std::string packageInfo[3];
        sqlite3* db;
        sqlite3_open(((std::string)VAR_PATH + "/packages.db").c_str(), &db);
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, "SELECT * FROM packages", -1, &stmt, 0);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            for(uint8_t i=0; i<3; i++)
                packageInfo[i] = (std::string)reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
                cout << left
                         << packageInfo[0] << setw(24-packageInfo[0].length()) << ""
                         << packageInfo[1] << setw(10-packageInfo[1].length()) << ""
                         << packageInfo[2] << setw(10-packageInfo[2].length()) << ""
                         << endl;
        }
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
    return 0;
}
