
#include <iostream>
// Для простоты
#include <bits/stdc++.h>

using namespace std;

bool Help() {
    #if LOGO_ENABLE == 1
    cout << "\n ██████████  ███  ███    ████   ███████    ██████" << endl
    << "███      ███ ████  ███  ███   ███     ██      █████" << endl
    << "███     ███ ███ ███ █████     ██       ██      █████" << endl
    << "█████████  ███   ███ ██████   ██        ██    ████" << endl
    << "████      ██████████ ███  ███ ███        ██" << endl
    << " ████    ████     ███ ███  ███ ███      ███  ███" << endl
    << " ███    ███        ███ ██  ████ ████████    ██" << endl;
    #endif
    cout << "[Pako!] v" << (string)VERSION << endl
    << "Usage: pako <action> package" << endl
    << "[    -i    install a pako package     ]" << endl
    << "[    -r    remove a pako package      ]" << endl
    << "[    -l    list installed packages    ]" << endl
    << "[    -h    print help message         ]" << endl;
    return false;
}