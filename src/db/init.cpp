#include <string>

#include <sqlite3.h>

#include "../essential/o.h"
#include "database.h"

extern Output output;
Database db;

void Database::init() {
    sqlite3* db;
    
    if (sqlite3_open(dbPath.c_str(), &db)) {
        output.error("init.c:init_database: SQLite error: " + (string)sqlite3_errmsg(db));
        exit(1);
    }

    if(sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS packages (name TEXT, infofilepath TEXT, filelistpath TEXT);", 0, 0, 0)) {
        output.error("init.c:init_database: SQLite error: " + (string)sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_close(db);
    return;
}