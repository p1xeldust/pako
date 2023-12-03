#include <archive.h>
#include <archive_entry.h>

#include <iostream>
#include <fstream>
#include <regex>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <sys/utsname.h>
#include <cstring>
#include <filesystem>
#include <shared_mutex>
#include <string>
#include <sstream>
#include <exception>

#include "pako.hpp"

using std::string, std::cin, std::to_string, std::vector, std::exception, std::fstream, std::exception, std::ios;
namespace fs = std::filesystem;

extern int8_t remove(std::vector<std::string> arguments);

std::string package_data[3];

int8_t installPackage(const std::vector<std::string> arguments) {
    #if NOSU == 0
    if(geteuid() != 0) {
        errormsg("Action requires superuser privileges.");
        return 127;
    }
    #endif
    // Проверка на обычный файл
    for(const auto &file : arguments)
        if(!fs::is_regular_file(file)) {
            errormsg((file + " is not a file.").c_str());
            return 1;
        }
    // Создание временных папок с содержимым распаковываемых файлов
    for(size_t i=0; i<arguments.size(); i++)
        if(!make_tmp((string)"package" + to_string(i)))
            return 2;
    // Распаковка и проверка контрольного файла
    for(size_t i=0; i<arguments.size(); i++) {
        string package_data[3];
        if(!unpack_package_archive(arguments[i], (string)TMP_PATH + "/package" + to_string(i))) {
            errormsg(("Can't extract %s.\n"), arguments[i].c_str());
            return 3;
        }
        if(!(fs::is_directory((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO") ||
             fs::is_directory((string)TMP_PATH + "/package" + to_string(i) + "/package/source") ||
             fs::is_regular_file((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info")  ||
             fs::is_regular_file((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/files") ||
             read_package_data((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info", package_data))) {
            errormsg("Can't process %s: Not a pako package.\n", arguments[i].c_str());
            return 4;
        }
    }
    for(size_t i=0; i<arguments.size(); i++) {
        string package_data[3];
        read_package_data((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info", package_data);
        if(isInDatabase(package_data[0].c_str())) {
            msg("Reinstalling %s...", package_data[0].c_str());
            removePackage({package_data[0], "--force-remove"});
        }
        if(!check_architecture(package_data[1])) {
            msg("Incompatible architecture detected for %s:%s (%s)\nContinue? [y/N] ", package_data[0].c_str(), package_data[1].c_str(), package_data[2].c_str());
            string ans;
            cin >> ans;
            if(!(ans == "y" || ans == "yes" || ans == "Y" || ans == "YES")) {
                msg("Cancelled.");
                return 5;
            }
        }
        
        if(!check_dependencies((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info") ||
           !check_conflicts((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info"))
           return 6;
    }
    /* Копируем контрольные файлы в VAR_PATH */
    for(size_t i=0; i<arguments.size(); i++) {
		string package_data[3];
        read_package_data((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info", package_data);

		try {
			for (const auto& entry : fs::recursive_directory_iterator((string)TMP_PATH + "/package" + std::to_string(i) + "/package/PAKO")) {
				if(entry.path().filename() == "files")
					continue;
				fs::copy(entry.path(), (string)VAR_PATH + "/control/" + package_data[0] + "." + entry.path().filename().string(), fs::copy_options::overwrite_existing);
			}
		}
		catch(const exception& e) {
		        errormsg("can't copy package control files.");
            #ifdef DEBUG
			debugmsg("CopyControlFiles: %s\n", e.what());
            #endif
			return 7;
		}
        msg("Installing %:%s (%s)\n", package_data[0].c_str(), package_data[1].c_str(), package_data[2].c_str());
		if(fs::exists((string)VAR_PATH +"/control/" + package_data[0] + ".install"))
			system(("sh " + (string)VAR_PATH +"/control/" + package_data[0] + ".install" + " --preinst").c_str());
	}

	// Копируем содержимое всех распакованных пакетов в префикс
	for (size_t i = 0; i < arguments.size(); i++) {
        string package_data[3]; 
        read_package_data((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info", package_data);
        fstream filesList((string)VAR_PATH + "/control/" + package_data[0] + ".files", ios::app);
        for (const auto& entry : fs::recursive_directory_iterator((string)TMP_PATH + "/package" + to_string(i) + "/package/source")) {
            string path = entry.path().string().substr(((string)TMP_PATH + "/package" + to_string(i) + "/package/source").size());
            if(fs::is_directory(entry.path()) && !fs::is_symlink(entry.path()) && !fs::exists((string)PREFIX + "/" + path)) {
                #ifdef DEBUG
                debugmsg("Created directory: %s\n", ((string)PREFIX + "/" + path).c_str());
                #endif
                fs::create_directories((string)PREFIX + "/" + path);
            } 
            else {
                try {
                    if(!fs::exists((string)PREFIX + "/" + path) && !fs::is_directory(entry.path())) {
                        filesList << path << "\n";
                        #ifdef DEBUG
                        debugmsg("Creating file: %s\n", ((string)PREFIX + "/" + path).c_str());
                        #endif
                        fs::copy(entry.path(), (string)PREFIX + "/" + path, fs::copy_options::overwrite_existing | fs::copy_options::copy_symlinks);
                        #ifdef DEBUG
                        debugmsg("Created file: %s\n", ((string)PREFIX + "/" + path).c_str());
                        #endif
                    } else if(fs::is_directory((string)PREFIX + "/" + path)) {
                        #ifdef DEBUG
                        debugmsg("Creating directory: %s\n", ((string)PREFIX + "/" + path).c_str());
                        #endif
                        fs::create_directories((string)PREFIX + "/" + path);
                        #ifdef DEBUG
                        debugmsg("Created directory: %s\n", ((string)PREFIX + "/" + path).c_str());
                        #endif
                    }
                }
                catch (std::filesystem::filesystem_error const& e) {
                    if (e.code().value() == 22) {
                        continue;
                    } else {
                        errormsg("Can't copy package sources.\n");
                        #ifdef DEBUG
                        debugmsg("Filesystem error message %s\n", e.what());
                        #endif
                        return 9;
                    }
                }
            }
        }
        filesList.close();
        debugmsg(("Adding %s to database...\n", package_data[0].c_str()));
        if(!addPackageToDatabase(package_data[0].c_str(), package_data[1].c_str(), package_data[2].c_str(), ((string)VAR_PATH + "/control/" + package_data[0] + ".info").c_str(), ((string)VAR_PATH + "/control/" + package_data[0] + ".files").c_str())) {
            errormsg("Can't add %s to database\n", package_data[0].c_str());
            return 10;
        }
        
        if(fs::exists((string)VAR_PATH +"/control/" + package_data[0] + ".install")) {
            msg("Configuring %s:%s (%s)\n", package_data[0].c_str(), package_data[1].c_str(), package_data[2].c_str());
            system(("sh " + (string)VAR_PATH +"/control/" + package_data[0] + ".install" + " --postinst").c_str());
        }
        msg("installed %s:%s (%s)\n", package_data[0].c_str(), package_data[1].c_str(), package_data[2].c_str());
    }
    clear_tmp();
    return 0;
}
