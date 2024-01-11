#include <iostream>
#include <string>

#include <sqlite3.h> // Beloved SQLite3

#include "../package/package.h"
#include "../essential/o.h"
#include "database.h"

using std::string;

extern Output output;

int Database::add(Package& package) {
    sqlite3* db; sqlite3_stmt* stmt;
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        output.error("add.c: Cant open database: " + (string)sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }
    if (sqlite3_prepare_v2(db, "INSERT INTO packages VALUES (?, ?, ?);", -1, &stmt, 0) != SQLITE_OK) {
        output.error("add.c: Unable to insert package into database. SQLite error: " + (string)sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    sqlite3_bind_text(stmt, 1, package.name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, package.files.specFile.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, package.files.listFile.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        output.error("add.c: Unable to insert package into database. SQLite3 error: " + (string)sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 1;
}