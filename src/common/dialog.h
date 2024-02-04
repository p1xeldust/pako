#ifndef PAKO_DIALOG_H
#define PAKO_DIALOG_H

#include "../package/package.h"

class Dialog {
private:
    void header();
    void footer();
public:
    bool SolveArchitecture(Package& currentPackage, const char* hostArchitecture);
    bool solveCopy(Package& currentPackage, const char* oldFilePath);

    int SolveArchitecture(Package& currentPackage, std::string hostArchitecture) {
        return SolveArchitecture(currentPackage, hostArchitecture.c_str());
    }
    int solveCopy(Package& currentPackage, std::string oldFilePath) {
        return solveCopy(currentPackage, oldFilePath.c_str());
    }
};

extern Dialog dialog;

#endif