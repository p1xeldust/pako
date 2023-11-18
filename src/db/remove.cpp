#include <sqlite3.h>
#include <iostream>
#include <string>

#include "database.h"

bool Database::removePackage(std::string packageName) {
    sqlite3_open(((std::string)VAR_PATH + "/packages.db").c_str(), &db);
    sqlite3_stmt* stmt;

    sqlite3_prepare_v2(db, "DELETE FROM packages WHERE name = ?;", -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, packageName.c_str(), -1, SQLITE_STATIC);
    if(sqlite3_step(stmt) != SQLITE_DONE) {
        out.errormsg("Failed to remove package from database");
        out.debugmsg((std::string)sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}
