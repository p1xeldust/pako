#include <iostream>
#include <string>
#include <vector>

#include "../db/database.h"

#include "../essential/o.h"
extern Output output;

#include "../pako.h"

using std::vector, std::string, std::cout;

int Pako::list(vector<string> packages) {
    auto it = packages.begin();

    while (it != packages.end()) {
        const auto& packageit = *it;

        if (!db.isIn(packageit)) {
            output.warn(packageit + " is not installed, skipping.");
            it = packages.erase(it);
        }
        else {
            ++it;
        }
    }
    for(const auto& packageit : packages) {
        Package package;
        db.getData(packageit, package);
        cout << package.name << " " << package.version << " " << package.arch.name << " " << package.description << "\n";
    }
    return 0;
}