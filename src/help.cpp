
#include <iostream>

using namespace std;

void Help() {
    #if LOGO_ENABLE == 1
    cout << endl
    << "   ███████ ███ ███  ████ ████████    █████" << endl
    << "  ███  ███ ███  ███ ███  ███     ███  █████" << endl
    << "  ███ ███ ██ ███ █████    ███     ███  █████" << endl
    << "  ██████ ███  ███ ██████   ███     ███  ███" << endl
    << " ███    ██████████ ███  ███ ███     ███ " << endl
    << " ███   ███     ███ ███  ███ ███     ███  ███" << endl
    << "███   ███       ███ ███  ███ █████████  ███" << endl
    << endl;
    #endif
    cout << "[Pako!] v" << (string)VERSION << endl
    << "Usage: pako <action> package" 
    << endl
    << endl
    << "  -i | install a pako package" 
    << endl
    << "  -r | remove a pako package" 
    << endl
    << "  -l | list installed packages" 
    << endl
    << "  -h | print help message" 
    << endl
    << "  -v | print help message " 
    << endl;
}

void Version() {
    cout << "[Pako!] version " << (string)VERSION << endl;
}
