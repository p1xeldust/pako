#include <filesystem>

#include "../db/database.h"
#include "../output/print.h"

#ifndef PACKAGE_H
#define PACKAGE_H

class Package {
private:
    Print out;
    Database db;
public:
    bool make_tmp(std::string path);
    bool clear_tmp();
    bool unpack_package_archive(std::string source, std::string destination);
    bool read_package_data(std::string dataFilePath, std::string package_data[3]);
    bool check_dependencies(std::string data_file_path);
    bool check_conflicts(std::string data_file_path);
    bool check_architecture(std::string package_arch);
    void remove_package_source(std::string list_file_path);
    bool solve_dependencies(std::string package_name);
};

#endif
