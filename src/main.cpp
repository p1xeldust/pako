#include "essential/o.h"
#include "db/database.h"
#include "pako.h"

extern Database db;
extern Output output;
Pako pako;

int main(int argc, char** argv) {
    vector<string> arguments;
    if(argc < 2) {
        output.msg("No help now, sorry!");
        return 1;
    }
    string action = argv[1];
    db.init();

    for(int i = 2; i<argc; i++) {
        arguments.push_back(argv[i]);
    }
    if(action == "-i")
        pako.install(arguments);
    else if(action == "-l")
        pako.list(arguments);
    else if(action == "-r")
        pako.remove(arguments);
    else
        output.msg("No help now, sorry!");
}