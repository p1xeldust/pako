#include <sqlite3.h>
#include <string>

#include "../package/package.h"
#include "../essential/o.h"
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
