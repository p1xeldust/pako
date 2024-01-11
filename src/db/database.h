#ifndef PAKO_DATABASE_H
#define PAKO_DATABASE_H

#include <string>
#include "../package/package.h"

class Database {
private:
    string dbPath = (string)VAR_PATH + "/packages.db";
public:
    void init();
    int add(Package& package);
    int isIn(string packageName);
    int getData(string packageName, Package& package);
    int remove(string PackageName);
};

extern Database db;

#endif