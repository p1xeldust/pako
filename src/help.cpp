
#include <iostream>
#include <ostream>

using namespace std;

bool Help() {
    #if LOGO_ENABLE == 1
    cout << ""
    #endif
    cout << "[Pako!] v" << (string)VERSION << endl
    << "Usage: pako <action> package" << endl
    << "[    -i    install a pako package     ]" << endl
    << "[    -r    remove a pako package      ]" << endl
    << "[    -l    list installed packages    ]" << endl
    << "[    -h    print help message         ]" << endl;
    return false;
}