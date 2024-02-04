#include <filesystem>
#include <algorithm>
#include <fstream>
#include <vector>
#include <string>

#include "package.h"
#include "exec.h"
#include "../db/database.h"
#include "../common/output.h"

using std::string, std::vector, std::ifstream, std::filesystem::read_symlink,
        std::filesystem::path, std::filesystem::copy, 
        std::filesystem::create_directories, std::filesystem::is_regular_file,
        std::filesystem::is_empty, std::filesystem::is_symlink,
        std::remove, std::filesystem::remove, std::filesystem::exists;

int checkDepsOnRemove(Package& package) {
    std::ifstream dbFile(configParams.dbPath);
    std::string line;
    
    while (std::getline(dbFile, line)) {
        std::istringstream iss(line);
        std::string word;
        
        while (iss >> word) {
            if (word == "dependencies") {
                size_t pos = line.find(package.name);
    
                if (pos != std::string::npos) {
                    return -1;
                } else {
                    continue;
                }
            }
        }
    }

    return 0;
}

int Remove(vector<string> packages) {
    auto it = packages.begin();

    while (it != packages.end()) {
        const auto& packageit = *it;
        if (!db.IsIn(packageit)) {
            output.warn(packageit + " is not installed, skipping.");
            it = packages.erase(it);
        }
        else {
            ++it;
        }
    }
    for(const auto& packageit : packages) {
        Package package = db.GetPackage(packageit);
        if(checkDepsOnRemove(package) == -1) {
            output.warn("Ignoring " + package.name);
            continue;
        }
        if(exists(package.files.scriptFilePath))
            execScript(package.files.scriptFilePath, PRE_REMOVE);
        ifstream listFile(package.files.listFilePath);
        for(string line; getline(listFile, line);) {
            std::string filePath = configParams.prefixPath.string() + "/" + line;
            if(is_regular_file(filePath))
                std::filesystem::remove(filePath);
        }
        listFile.seekg(0, std::ios::beg);
        for(string line; getline(listFile, line);) {
            std::string filePath = configParams.prefixPath.string() + "/" + line;
            if(is_empty(filePath))
                std::filesystem::remove(filePath);
        }
        listFile.seekg(0, std::ios::beg);
        for(string line; getline(listFile, line);) {
            std::string filePath = configParams.prefixPath.string() + "/" + line;
            if(is_symlink(filePath) && !exists(read_symlink(filePath)))
                std::filesystem::remove(filePath);
        }
        listFile.close();
        std::filesystem::remove(package.files.listFilePath);
        if(exists(package.files.scriptFilePath)) {
            execScript(package.files.scriptFilePath, POST_REMOVE);
            std::filesystem::remove(package.files.scriptFilePath);
        }
        db.RemovePackage(package);
        output.msg("Removed " + package.name + " " + package.version + " (" + package.arch.name + ")");
    }
    return 0;
}