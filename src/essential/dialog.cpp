#include <iostream>
#include <string>

#include "../package/package.h"
#include "dialog.h"

Dialog dialog;

using std::string, std::cout, std::cin, std::getline;

int Dialog::solve_copy(string oldFilePath, Package& package) {
    string choice;
    while (1) {
        cout << "---   ===   DIALOG   ===   ---\n"
            << "File: " << oldFilePath << "\n"
            << "This file already exists but also provided by current '" << package.name << "'\n"
            << "What do you want to do?\n"
            << "  I / i  - install newer version of file\n"
            << "  K / k - keep current version\n"
            << "  A / a - Abort installation completely\n"
            << "---   ===   DIALOG   ===   ---\n"
            << "Choice: ";
        getline(cin, choice);
        if (choice == "K" || choice == "k")
            return 0;
        else if (choice == "I" || choice == "i")
            return 1;
        else if (choice == "A" || choice == "a")
            return -1;
        else
            continue;
    }
}

int Dialog::solve_arch(Package& package) {
    string choice;
    while (1) {
        cout << "---   ===   DIALOG   ===   ---\n"
            << "Package '" << package.name << "' have foreign architecture:\n"
            << "Arch: " << package.arch.name << "\n"
            << "What do you want to do?\n"
            << "  I / i - install this package\n"
            << "  A / a - Abort installation of this package\n"
            << "--- === DIALOG === ---\n"
            << "Choice: ";
        getline(cin, choice);
        if (choice == "I" || choice == "i")
            return 0;
        else if (choice == "A" || choice == "a")
            return -1;
        else
            continue;
    }
}