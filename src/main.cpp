#include "common/dialog.h"
#include "common/config.h"
#include "common/output.h"
#include "db/database.h"

#include "package/main.h"

Dialog dialog;
Output output;
Database db;


int main(int argc, char** argv) {
    configParams.ReadConfigFile("/etc/pako/pako.conf");
    std::vector<std::string> arguments;
    if(argc < 2) {
        Help();
        return 1;
    }
    std::string action = argv[1];

    for(int i = 2; i<argc; i++) {
        arguments.push_back(argv[i]);
    }
    if(action == "-i")
        Install(arguments);
    else if(action == "-l")
        List(arguments);
    else if(action == "-r")
        Remove(arguments);
    else if(action == "-v")
        Version();
    else
        Help();
}
