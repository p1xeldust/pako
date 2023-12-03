#include <sqlite3.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../output/print.h"

#ifndef DATABASE_H
#define DATABASE_H

#ifdef __cplusplus
extern "C" {
#endif
    bool initDatabase();
    bool addPackageToDatabase(const char* packageName, const char* packageArch, const char* packageVersion, const char* listPath, const char* infoPath);
    bool isInDatabase(const char* packageName);
    bool removePackageFromDatabase(const char* packageName);
    bool readDBPackageData(const char* packageName, char* packageData[5]);
#ifdef __cplusplus
}
#endif
#endif
