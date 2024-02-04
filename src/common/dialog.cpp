#include <iostream>

#include "output.h"
#include "dialog.h"

void Dialog::header()
{
    output.msg("-- == Dialog == --");
}

bool Dialog::SolveArchitecture(Package &currentPackage, const char *hostArchitecture)
{
    output.warn("dialog.cpp: Dialog triggered");
    std::string answer;

    for (;;)
    {
        header();
        std::cout
            << "Current package " << currentPackage.name << " is supposed for " << currentPackage.arch.name << "\n"
            << "Your system is " << hostArchitecture << "\n"
            << "  Y - install package\n"
            << "  N - skip package\n\n"
            << "[Y/y][N/n] > ";
        std::getline(std::cin, answer);
        if (answer[0] == 'Y' || answer[0] == 'y')
            return false;
        else if (answer[0] == 'N' || answer[0] == 'n')
            return true;
        else
            continue;
    }
    return false;
}

bool Dialog::solveCopy(Package &currentPackage, const char *oldFilePath)
{
    output.warn("dialog.cpp: Dialog triggered");
    std::string answer;

    for (;;)
    {
        header();
        std::cout
            << "File: " << oldFilePath << "\n"
            << "This file already exists but also provided by package '" << currentPackage.name << "'\n"
            << "What do you want to do?\n"
            << "  I - install newer version of file\n"
            << "  K - keep current version\n"
            << "[I/i][K/k] > ";
        std::getline(std::cin, answer);
        if (answer[0] == 'I' || answer[0] == 'i')
            break;
        else if (answer[0] == 'K' || answer[0] == 'k')
            return true;
        else
            continue;
    }
    return false;
}