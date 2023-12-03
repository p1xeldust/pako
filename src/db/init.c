#include <sqlite3.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "database.h"
#include "../output/print.h"

bool initDatabase() {
    sqlite3 *db;
    char database_file_path[strlen(VAR_PATH) + strlen("packages.db") + 2];
    sprintf(database_file_path, "%s/%s", VAR_PATH, "packages.db");
    sqlite3_open(database_file_path, &db);
    if(sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS packages (name TEXT, version TEXT, arch TEXT, infofilepath TEXT, filelistpath TEXT);", 0, 0, 0)) {
        errormsg("Unable to init database.\n");
        debugmsg("%s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    } else
        debugmsg("Created packages table");
    sqlite3_close(db);
    return 1;
}
