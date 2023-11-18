#include <sqlite3.h>
#include <string>

#include "database.h"

bool Database::isInDatabase(std::string packageName) {
    sqlite3_open(((std::string)VAR_PATH + "/packages.db").c_str(), &db);
    sqlite3_stmt* stmt;

    sqlite3_prepare_v2(db, "SELECT * FROM packages WHERE name = ?;", -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, packageName.c_str(), -1, SQLITE_STATIC);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        out.debugmsg("isInDatabase: Found: " + (std::string)reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}

std::string * Database::readDBPackageData(std::string packageName) {
    std::string* packageData = new std::string[5];
    out.debugmsg("[readDBPackageData] Opening " + (std::string)VAR_PATH + "/packages.db");
    sqlite3_open(((std::string)VAR_PATH + "/packages.db").c_str(), &db);
    sqlite3_stmt* stmt;

    sqlite3_prepare_v2(db, "SELECT * FROM packages WHERE name = ?;", -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, packageName.c_str(), -1, SQLITE_STATIC);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        out.debugmsg("readDBPackageData: Found: " + (std::string)reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        for(uint8_t i=0; i<5; i++)
            packageData[i] = (std::string)reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
        for(uint8_t i=0; i<5; i++)
            out.debugmsg("readPackageData: packageData[" + std::to_string(i) + "]: " + packageData[i]);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return packageData;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return packageData;
}
