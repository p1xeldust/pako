#include <vector>
#include <fstream>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "package.h"

bool Package::solveDeps(std::string packageName) {
    std::vector<std::string> deps;
    sqlite3* db; sqlite3_stmt* stmt;
    if (sqlite3_open(((std::string)VAR_PATH + "/packages.db").c_str(), &db)) {
        out.debugmsg("solveDeps: Can't open database: " + (std::string)sqlite3_errmsg(db));
        return 0;
    }
    if (sqlite3_prepare_v2(db, "SELECT * FROM packages;", -1, &stmt, 0) != SQLITE_OK) {
        out.debugmsg("solveDeps: SQL error: " + (std::string)sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        if(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)) == ((std::string)VAR_PATH + "/control/" + packageName + ".info").c_str())
            continue;
        std::string diffPackagesInfoFile = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        out.debugmsg("SolveDeps: diffPackagesInfoFile: " + diffPackagesInfoFile);
        out.debugmsg("SolveDeps: curInfoFile: " + (std::string)VAR_PATH + "/control/" + packageName + ".info");
        std::ifstream dataFile(diffPackagesInfoFile, std::ios::in);
        for(std::string line; getline(dataFile, line); dataFile.good()) {
            if(line.find("deps:") != std::string::npos) {
                std::istringstream depList(line.substr(line.find(':') + 1).erase(0, line.find_first_not_of(" \t")));
                for(std::string dep; depList >> dep;)
                    if(dep == packageName)
                        deps.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
                }
          }
    }
    if(deps.size() <= 0) {
        out.debugmsg("SolveDeps: solved deps!");
        return true;
    } else {
        out.errormsg("Unable to solve dependencies:");
        for(size_t i=0; i < deps.size(); i++)
            out.msg("Package '" + deps[i] + "' depends on " + packageName);
        out.msg("Use --force-remove to avoid this check.");
        return false;
    }
    sqlite3_close(db);
    return 1;
}


void Package::removePackageSource(std::string listFilePath) {
    std::vector<std::string> files;
    std::ifstream listFile(listFilePath);
    for(std::string line; getline(listFile, line); listFile.good()) {
        files.push_back((std::string)PREFIX + "/" + line);
        //out.debugmsg(" Added " + (std::string)PREFIX + "/" + line + " to remove list");
    }
    /* Удаление файлов */
    for(const auto file : files) {
        if(std::filesystem::is_regular_file(file)) {
            std::filesystem::remove(file);
            out.debugmsg("Removed file " + file);
        }
    }
    for(const auto file : files) {
        if(std::filesystem::is_directory(file) && std::filesystem::is_empty(file)) {
            std::filesystem::remove(file);
            out.debugmsg("Removed directory " + file);
        }
    }
    for(const auto file : files) {
        if(std::filesystem::is_symlink(file)) {
            std::filesystem::remove(file);
            out.debugmsg("Removed symlink " + file);
        }
    }
    for(const auto file : files) {
        if(std::filesystem::is_symlink(file) && std::filesystem::exists(std::filesystem::read_symlink(file))) {
            std::filesystem::remove(file);
            out.debugmsg("Removed symlink " + file);
        }
    }
}
