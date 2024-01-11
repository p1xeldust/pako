#include "db/database.h"
#include "pako.h"

extern Database db;
Pako pako;

int main() {
    db.init();
    vector<string> packages = {"archive.tar"};
    vector<string> packagesdel = {"flag"};
    pako.install(packages);
    pako.list(packagesdel);
    pako.remove(packagesdel);
}