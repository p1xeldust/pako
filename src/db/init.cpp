#include <sqlite3.h>
#include <string>

#include "database.h"

bool Database::initDatabase() {
    sqlite3_open(((std::string)VAR_PATH + "/packages.db").c_str(), &db);
    if(sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS packages (name TEXT, version TEXT, arch TEXT, infofilepath TEXT, filelistpath TEXT);", 0, 0, 0)) {
        out.errormsg("Unable to create table:");
        out.debugmsg((std::string)sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    } else
        out.debugmsg("Created packages table");
    sqlite3_close(db);
    return 1;
}
