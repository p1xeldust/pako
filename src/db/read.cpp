#include <iostream>
#include <string>

#include <sqlite3.h> // Beloved SQLite3

#include "../package/package.h"
#include "../package/parse.h"
#include "../essential/o.h"

#include "database.h"

extern Database db;
extern Output output;

using std::string;

int Database::isIn(string packageName)
{
    sqlite3* db;
    sqlite3_stmt* stmt;

    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK)
    {
        output.error("read.cpp:isInDB: Cant open database: " + (string)sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    if (sqlite3_prepare_v2(db, "SELECT * FROM packages WHERE name = ?;", -1, &stmt, 0) != SQLITE_OK)
    {
        output.error("read.cpp:isInDB: Cant open database: " + (string)sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    sqlite3_bind_text(stmt, 1, packageName.c_str(), -1, SQLITE_STATIC);

    int result = sqlite3_step(stmt);
    if (result == SQLITE_ROW)
    {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    }

    if (result != SQLITE_DONE)
    {
        output.error("read.cpp:isInDB: Cant open database: " + (string)sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}

int Database::getData(string packageName, Package& package)
{
    sqlite3* db;
    sqlite3_stmt* stmt;
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK)
    {
        output.error("read.cpp:getData: isInDB: Cant open database: " + (string)sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    if (sqlite3_prepare_v2(db, "SELECT * FROM packages WHERE name = ?;", -1, &stmt, 0) != SQLITE_OK) {
        output.error("read.cpp:getData: Error selecting package from db. SQLite error: " + (string)sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        exit(1);
    }

    sqlite3_bind_text(stmt, 1, packageName.c_str(), -1, SQLITE_STATIC);

    int step_result = sqlite3_step(stmt);
    if (step_result == SQLITE_ROW) {
        package.name = (char*)sqlite3_column_text(stmt, 0);
        package.files.specFile = (char*)sqlite3_column_text(stmt, 1);
        package.files.listFile = (char*)sqlite3_column_text(stmt, 2);
        package.files.installScript = (char*)sqlite3_column_text(stmt, 3);
    }
    else if (step_result != SQLITE_DONE) {
        output.error("read.cpp:getData: Error selecting package from db. SQLite error: " + (string)sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        exit(1);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return parseSpecs(package.files.specFile, package);
}
