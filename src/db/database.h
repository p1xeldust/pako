#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>

#include "../output/print.h"

class Database {
private:
    sqlite3 *db;
    Print out;
public:
    bool initDatabase();
    bool addPackage(std::string packageData[3], std::string listPath, std::string infoPath);
    bool isInDatabase(std::string packageName);
    bool removePackage(std::string packageName);
    std::string * readDBPackageData(std::string packageName);
};

#endif
