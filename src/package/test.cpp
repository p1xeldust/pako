#include <vector>

#include "package.h"

extern Database db;
extern int8_t Install(std::vector<std::string> arguments);

int main() {
    db.initDatabase();
    Install({"package.pako"});
}
