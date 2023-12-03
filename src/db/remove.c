#include <sqlite3.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "database.h"

bool removePackageFromDatabase(const char* package_name) {
    sqlite3* db;
    char database_file_path[strlen(VAR_PATH) + strlen("packages.db") + 2];
    sprintf(database_file_path, "%s/%s", VAR_PATH, "packages.db");
    sqlite3_open(database_file_path, &db);
    sqlite3_stmt* stmt;
    bool return_value = 0;

    sqlite3_prepare_v2(db, "DELETE FROM packages WHERE name = ?;", -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, package_name, -1, SQLITE_STATIC);
    if(sqlite3_step(stmt) != SQLITE_DONE) {
        errormsg("Failed to remove package from database");
        debugmsg(sqlite3_errmsg(db));
        return_value++;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return return_value;
}
