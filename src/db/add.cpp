#include <sqlite3.h>
#include <string>

#include "database.h"

bool Database::addPackage(std::string packageData[3], std::string listPath, std::string infoPath) {
    sqlite3_open(((std::string)VAR_PATH + "/packages.db").c_str(), &db);
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db, "INSERT INTO packages VALUES (?, ?, ?, ?, ?);", -1, &stmt, 0) != SQLITE_OK) {
        out.errormsg("Unable to insert package into database");
        out.debugmsg(sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }
    for (uint8_t i = 0; i < 3; i++) {
        sqlite3_bind_text(stmt, i + 1, packageData[i].c_str(), -1, SQLITE_STATIC);
    }
    sqlite3_bind_text(stmt, 4, listPath.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, infoPath.c_str(), -1, SQLITE_STATIC);
    if(sqlite3_step(stmt) != SQLITE_DONE) {
        out.errormsg("Unable to insert package into database.");
        out.debugmsg((std::string)sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 0;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 1;
}
