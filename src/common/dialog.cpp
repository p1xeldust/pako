#include <iostream>

#include "output.h"
#include "dialog.h"

void Dialog::header()
{
    output.msg("-- == Dialog == --");
}

bool Dialog::SolveArchitecture(Package &currentPackage, const char *hostArchitecture)
{
    char answer;
    for (;;)
    {
        header();
        std::cout
            << currentPackage.name << " is for " << currentPackage.arch.name << "\n"
            << "Your system is " << hostArchitecture << "\n"
            << "  I - Install\n"
            << "  S - Skip\n\n"
            << "[Y/y][S/s] > ";
        std::cin >> answer;
        if (answer == 'Y' || answer == 'y')
            return false;
        else if (answer == 'S' || answer == 's')
            return true;
        else
            continue;
    }
    return false;
}

bool Dialog::solveCopy(Package &currentPackage, const char *oldFilePath)
{
    char answer;
    for (;;)
    {
        header();
        std::cout
            << "File: " << oldFilePath << " already exists but also provided by '" << currentPackage.name << "'\n"
            << "  I - install newer file\n"
            << "  K - keep current file\n"
            << "[I/i][K/k] > ";
        std::cin >> answer;
        if (answer == 'I' || answer == 'i')
            break;
        else if (answer == 'K' || answer == 'k')
            return true;
        else
            continue;
    }
    return false;
}