#include <filesystem>
#include <algorithm>
#include <fstream>
#include <vector>
#include <string>

#include "package.h"

#include "../db/database.h"
extern Database db;
#include "../essential/o.h"
extern Output output;


#include "../pako.h"

using std::string, std::vector, std::ifstream, std::filesystem::read_symlink,
        std::filesystem::path, std::filesystem::copy, 
        std::filesystem::create_directories, std::filesystem::is_regular_file,
        std::filesystem::is_empty, std::filesystem::is_symlink,
        std::remove, std::filesystem::remove, std::filesystem::exists;

int Pako::remove(vector<string> packages) {
    auto it = packages.begin();

    while (it != packages.end()) {
        const auto& packageit = *it;
        if (!db.isIn(packageit)) {
            output.warn(packageit + " is not installed, skipping.");
            it = packages.erase(it);
        }
        else {
            ++it;
        }
    }
    for(const auto& packageit : packages) {
        Package package;
        if(db.getData(packageit, package)== -1) {
            output.error("Cant get data from database\n");
            return 1;
        }
        ifstream listFile(package.files.listFile);
        for(string line; getline(listFile, line);) {
            if(is_regular_file((string)PREFIX + "/" + line))
                std::filesystem::remove((string)PREFIX + "/" + line);
        }
        listFile.seekg(0, std::ios::beg);
        for(string line; getline(listFile, line);) {
            if(is_empty((string)PREFIX + "/" + line))
                std::filesystem::remove((string)PREFIX + "/" + line);
        }
        listFile.seekg(0, std::ios::beg);
        for(string line; getline(listFile, line);) {
            if(is_symlink((string)PREFIX + "/" + line) && !exists(read_symlink((string)PREFIX + "/" + line)))
                std::filesystem::remove((string)PREFIX + "/" + line);
        }
        listFile.close();
        std::filesystem::remove(package.files.listFile);
        std::filesystem::remove(package.files.specFile);
        db.remove(package.name);
        output.msg("Removed " + package.name);
    }
    return 0;
}