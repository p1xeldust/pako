#include <string>
#include <iostream>

#include "database.h"

using namespace std;

class Database db;

int main() {
    string packageData[3] = {"flag", "1.0", "architecture"};

    db.initDatabase("packages.db");
    db.addPackage("packages.db", packageData, "Aboba", "Path2");
    std::cout << db.isInDatabase("packages.db", "flag") << std::endl;
    db.removePackage("packages.db","flag");
    std::cout << db.isInDatabase("packages.db","flag") << std::endl;
    return 0;
}

