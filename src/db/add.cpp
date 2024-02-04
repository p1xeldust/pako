#include <fstream>
#include <vector>
#include <iostream>

#include "../common/config.h"
#include "database.h"

void Database::AddPackage(Package &package)
{
    if(db.IsIn(package.name))
        db.RemovePackage(package);
    std::ofstream dbFile(configParams.dbPath, std::ios_base::app);
    dbFile
        << "name " << package.name
        << "\nversion " << package.version
        << "\narch " << package.arch.name
        << "\ntype " << package.meta
        << "\ndependencies";
    for (std::string dep : package.dependencies)
        dbFile << " " << dep;
    dbFile << "\nconflicts";
    for (std::string conflict : package.conflicts)
        dbFile << " " << conflict;
    dbFile
        << "\ndescription " << package.description
        << "\nlistfile " << package.files.listFilePath
        << "\nscriptfile " << package.files.scriptFilePath
        << "\nEOPE\n";
    dbFile.close();
}