/*
    Package structures and classes
    Written by Paul Goldstein, Jan 2024.

*/

#ifndef PAKO_PACKAGE_H
#define PAKO_PACKAGE_H

#include <linux/limits.h>
#include <vector>
#include <string>

using std::string, std::vector;

enum ArchMeta
{
    ARCH_NATIVE,
    ARCH_FOREIGN,
    ARCH_NONE
};

enum PackageMeta
{
    DEFAULT,
    RIDICULOUS,
    META
};

enum PackageScriptModes {
    PRE_INSTALL,
    POST_INSTALL,
    PRE_REMOVE,
    POST_REMOVE,
    RECONFIGURE
};

class Package {
private:
    struct ArchData {
        string name;
        enum ArchMeta meta;
    };
    struct Files {
        string specFile;
        string listFile;
        string installScript;
    };

public:
    string name;
    string version;
    string description;
    PackageMeta meta;
    ArchData arch;
    Files files;
    vector<string> dependencies;
    vector<string> conflicts;
};

#endif