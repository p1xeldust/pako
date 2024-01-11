#include <vector>
#include <string>

using std::string, std::vector;

class Pako {
public:
    int install(vector<string> packagesAsFiles);
    int remove(vector<string> packages);
    int list(vector<string> packages);
    void help();
    void version();
};