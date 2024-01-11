/*
    Package specs parser
    Written by Paul Goldstein, Jan 2024
*/

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

#include "../essential/o.h"
#include "package.h"
#include "specs.h"

extern Output output;

using std::ifstream, std::find, std::getline, std::vector, std::stoi, std::istringstream;

int parseSpecs(string specFilePath, Package& package) {
    ifstream specFile(specFilePath);

    if (!specFile) {
        output.error("parse.cpp: spec file not found");
        return -1;
    }
    for (string line; getline(specFile, line);) {
        string type = line.substr(0, line.find(" ")), value = line.substr(line.find(" ")+1);
        if (type == "name")
            package.name = value;
        else if (type == "version")
            package.version = value;
        else if (type == "arch")
            package.arch.name = value;
        else if (type == "type")
            package.meta = static_cast<PackageMeta>(stoi(value));
        else if (type == "dependencies") {
            istringstream dependencies(value);
            string dependency;
            while (dependencies >> dependency) {
                if(dependency == "dependencies")
                    continue;
                package.dependencies.push_back(dependency);
            }
        }
        else if (type == "conflicts") {
            istringstream conflicts(value);
            string conflict;
            while (conflicts >> conflict) {
                if(conflict == "conflicts")
                    continue;
                package.conflicts.push_back(conflict);
            }
        }
        else if (type == "description") {
            package.description = value;
        }
    }
    return checkSpecs(package);
}