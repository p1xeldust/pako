#include <sqlite3.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "database.h"
#include "../output/print.h"

bool addPackageToDatabase(const char* packageName, const char* packageArch, const char* packageVersion, const char* listPath, const char* infoPath) {
    sqlite3* db;
    char database_file_path[strlen(VAR_PATH) + strlen("packages.db") + 2];
    sprintf(database_file_path, "%s/%s", VAR_PATH, "packages.db");
    sqlite3_open(database_file_path, &db);
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db, "INSERT INTO packages VALUES (?, ?, ?, ?, ?);", -1, &stmt, 0) != SQLITE_OK) {
        errormsg("Unable to insert package into database.\n");
        #ifdef DEBUG
        debugmsg("addPackageToDatabase: %s\n", sqlite3_errmsg(db));
        #endif
        sqlite3_close(db);
        return 0;
    }
    
    sqlite3_bind_text(stmt, 1, packageName, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, packageArch, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, packageVersion, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, listPath, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, infoPath, -1, SQLITE_STATIC);
    if(sqlite3_step(stmt) != SQLITE_DONE) {
        errormsg("Unable to insert package into database.\n");
        #ifdef DEBUG
        debugmsg("addPackageToDatabase: %s\n", sqlite3_errmsg(db));
        #endif
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 0;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 1;
}
