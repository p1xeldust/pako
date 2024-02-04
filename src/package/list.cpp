#include <iostream>
#include <string>
#include <vector>

#include "../common/config.h"
#include "../common/output.h"
#include "../db/database.h"

using std::vector, std::string, std::cout;

int List(vector<string> packages) {
    auto it = packages.begin();

    while (it != packages.end()) {
        const auto& packageit = *it;

        if (!db.IsIn(packageit)) {
            output.warn(packageit + " is not installed, skipping.");
            it = packages.erase(it);
        }
        else {
            ++it;
        }
    }
    for(const auto& packageit : packages) {
        Package package = db.GetPackage(packageit);
        cout << package.name << " " << package.version << " " << package.arch.name << " " << package.description << "\n";
    }
    return 0;
}
