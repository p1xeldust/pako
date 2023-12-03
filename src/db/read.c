#pragma GCC diagnostic ignored "-Wreturn-local-addr"

#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "database.h"

bool isInDatabase(const char* packageName) {
    sqlite3* db;
    char database_file_path[strlen(VAR_PATH) + strlen("packages.db") + 2];
    sprintf(database_file_path, "%s/%s", VAR_PATH, "packages.db");
    sqlite3_open(database_file_path, &db);
    sqlite3_stmt* stmt;

    sqlite3_prepare_v2(db, "SELECT * FROM packages WHERE name = ?;", -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, packageName, -1, SQLITE_STATIC);
    if(sqlite3_step(stmt) == SQLITE_ROW) {
        #ifdef DEBUG
        debugmsg("isInDatabase: Found: %s\n",(char*)(sqlite3_column_text(stmt, 0)));
        #endif
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}

bool readDBPackageData(const char* packageName, char* packageData[5]) {
    sqlite3* db;
    char database_file_path[100]; // Предположим максимальная длина пути к файлу
    sprintf(database_file_path, "%s/%s", VAR_PATH, "packages.db");
    sqlite3_open(database_file_path, &db);
    sqlite3_stmt* stmt;

    sqlite3_prepare_v2(db, "SELECT * FROM packages WHERE name = ?;", -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, packageName, -1, SQLITE_STATIC);

    int result = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        for (int i = 0; i < 5; i++) {
            const unsigned char* text = sqlite3_column_text(stmt, i);
            if (text != NULL) {
                packageData[i] = malloc(strlen((const char*)text) + 1);
                if (packageData[i] != NULL) {
                    strcpy(packageData[i], (const char*)text);
                } else {
                    result = -1; // Ошибка при выделении памяти
                    break;
                }
            } else {
                packageData[i] = NULL;
            }
        }
        break; // Получена только первая строка
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return result;
}
