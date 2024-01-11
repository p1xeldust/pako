#include <sqlite3.h>
#include <algorithm> 
#include <fstream>
#include <string>

#include "../package/package.h"
#include "../essential/o.h"
#include "../package/parse.h"
#include "database.h"

extern Database db;
extern Output output;
using std::string;

int Database::remove(string PackageName) {
    sqlite3* db; sqlite3_stmt* stmt;

    if (sqlite3_open(dbPath.c_str(), &db)) {
        output.error("remove.c: Error while open database. SQLite error: " + (string)sqlite3_errmsg(db));
        exit(1);
    }

    if (sqlite3_prepare_v2(db, "DELETE FROM packages WHERE name = ?;", -1, &stmt, 0) != SQLITE_OK) {
        output.error("remove.c: Error deleting package from database. SQLite error: " + (string)sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    sqlite3_bind_text(stmt, 1, PackageName.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        output.error("remove.c: Error deleting package from database. SQLite error: " + (string)sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}

int Database::checkDepsOnRemove(Package& package, vector<string> packageList) {
    sqlite3* db;
    int fail = 0;
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        output.error("dependencies.cpp: Cant open database: " + (string)sqlite3_errmsg(db));
        return 1;
    }
    const char* query = "SELECT infofilepath FROM packages;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        output.error("dependencies.cpp: Can't get spec file path: " + (string)sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    // Выполнение запроса
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Package instPackage;
        parseSpecs((char*)sqlite3_column_text(stmt, 0), instPackage);
        for (const auto& dep : instPackage.dependencies) {
            for (const auto& packageFromList : packageList) {
                if (dep == package.name && std::find(packageList.begin(), packageList.end(), instPackage.name) == packageList.end()) {
                    output.error("Package '" + instPackage.name + "' depends on '" + package.name + "'. Remove it first.");
                    fail = 1;
                }
            }
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    if (fail)
        return -1;
    return 0;
}
