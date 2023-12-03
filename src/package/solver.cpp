#include <vector>
#include <fstream>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "package.h"

bool solve_dependencies(std::string packageName) {
    std::vector<std::string> dependencies;
    sqlite3* db; sqlite3_stmt* stmt;
    if (sqlite3_open(((std::string)VAR_PATH + "/packages.db").c_str(), &db)) {
        #ifdef DEBUG
        debugmsg(("solveDeps: Can't open database: " + (std::string)sqlite3_errmsg(db)).c_str());
        #endif
        return 0;
    }
    if (sqlite3_prepare_v2(db, "SELECT * FROM packages;", -1, &stmt, 0) != SQLITE_OK) {
        #ifdef DEBUG
        debugmsg(("solveDeps: SQL error: " + (std::string)sqlite3_errmsg(db)).c_str());
        #endif
        sqlite3_close(db);
        return 0;
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        if(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)) == ((std::string)VAR_PATH + "/control/" + packageName + ".info").c_str())
            continue;
        std::string diffPackagesInfoFile = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        #ifdef DEBUG
        debugmsg(("SolveDeps: diffPackagesInfoFile: " + diffPackagesInfoFile).c_str());
        debugmsg(("SolveDeps: curInfoFile: " + (std::string)VAR_PATH + "/control/" + packageName + ".info").c_str());
        #endif
        std::ifstream dataFile(diffPackagesInfoFile, std::ios::in);
        for(std::string line; getline(dataFile, line); dataFile.good()) {
            if(line.find("deps:") != std::string::npos) {
                std::istringstream depList(line.substr(line.find(':') + 1).erase(0, line.find_first_not_of(" \t")));
                for(std::string dep; depList >> dep;)
                    if(dep == packageName)
                        dependencies.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
                }
          }
    }
    if(dependencies.size() <= 0) {
        #ifdef DEBUG
        debugmsg("SolveDeps: solved deps!");
        #endif
        return true;
    } else {
        errormsg("Unable to solve dependencies:");
        for(size_t i=0; i < dependencies.size(); i++)
            msg(("Package '" + dependencies[i] + "' depends on " + packageName).c_str());
        msg("Use --force-remove to avoid this check.");
        return false;
    }
    sqlite3_close(db);
    return 1;
}


void remove_package_source(std::string listFilePath) {
    std::vector<std::string> files;
    std::ifstream listFile(listFilePath);
    for(std::string line; getline(listFile, line); listFile.good()) {
        files.push_back((std::string)PREFIX + "/" + line);
        //debugmsg((" Added " + (std::string)PREFIX + "/" + line + " to remove list").c_str());
    }
    /* Удаление файлов */
    for(const auto file : files) {
        if(std::filesystem::is_regular_file(file)) {
            std::filesystem::remove(file);
            #ifdef DEBUG
            debugmsg(("Removed file " + file).c_str());
            #endif
        }
    }
    for(const auto directory : files) {
        if(std::filesystem::is_directory(directory) && std::filesystem::is_empty(directory)) {
            std::filesystem::remove(directory);
            #ifdef DEBUG
            debugmsg(("Removed directory " + directory).c_str());
            #endif
        }
    }
    for(const auto file : files) {
        if(std::filesystem::is_symlink(file)) {
            std::filesystem::remove(file);
            #ifdef DEBUG
            debugmsg(("Removed symlink " + file).c_str());
            #endif
        }
    }
    for(const auto file : files) {
        if(std::filesystem::is_symlink(file) && !std::filesystem::exists(std::filesystem::read_symlink(file))) {
            std::filesystem::remove(file);
            #ifdef DEBUG
            debugmsg(("Removed dead symlink " + file).c_str());
            #endif
        }
    }
}
