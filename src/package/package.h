#ifndef PAKO_PACKAGE_H
#define PAKO_PACKAGE_H
#include <string>
#include <vector>

enum execMode {
    PRE_INSTALL,
    POST_INSTALL,
    PRE_REMOVE,
    POST_REMOVE,
    RECONFIGURE
};

enum packageMetaType {
    PACKAGE_DEFAULT, 
    PACKAGE_META,
    PACKAGE_RIDICULOUS
};

enum architectureMetaType {
    ARCH_NATIVE,
    ARCH_FOREIGN,
    ARCH_NONE
};

class Package {
    private:
        struct architectureType {
            std::string name;
            enum architectureMetaType meta;
        };

        struct packageFiles {
            std::string scriptFilePath = "";
            std::string listFilePath = "";
        };

    public:
        bool skipcurrent = 0;
        std::string name;
        std::string version;
        enum packageMetaType meta;
        architectureType arch;
        std::vector<std::string> dependencies;
        std::vector<std::string> conflicts;
        std::string description;
        packageFiles files;
};

#endif