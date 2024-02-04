#include "common/dialog.h"
#include "common/config.h"
#include "common/output.h"
#include "db/database.h"

#include "package/install.h"

Dialog dialog;
Output output;
Database db;
ConfigParams configParams;

int main(int argc, char** argv) {
    configParams.ReadConfigFile("pako.conf");
    std::vector<std::string> arguments;
    if(argc < 2) {
        output.msg("No help now, sorry!");
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
    else
        output.msg("No help now, sorry!");
}
