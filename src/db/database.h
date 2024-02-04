#ifndef PAKO_DB_H
#define PAKO_DB_H

#include "../common/config.h"
#include "../package/package.h"

class Database
{
public:
    void AddPackage(Package &package);
    Package GetPackage(std::string packageName);
    bool IsIn(std::string packageName);
    void RemovePackage(Package &package);

    bool IsIn(Package &package)
    {
        return IsIn(package.name);
    }

    Package GetPackage(Package &package)
    {
        return GetPackage(package.name);
    }
};

extern Database db;

#endif