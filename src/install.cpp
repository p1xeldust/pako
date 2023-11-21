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

int8_t Pako::install(const std::vector<std::string> arguments) {
    #if NOSU == 0
    if(geteuid() != 0) {
        out.errormsg("Action requires superuser privileges.");
        return 127;
    }
    #endif
    // Проверка на обычный файл
    for(const auto &file : arguments)
        if(!fs::is_regular_file(file)) {
            out.errormsg(file + " is not a file.");
            return 1;
        }
    // Создание временных папок с содержимым распаковываемых файлов
    for(size_t i=0; i<arguments.size(); i++)
        if(!pkg.make_tmp((string)"package" + to_string(i)))
            return 2;
    // Распаковка и проверка контрольного файла
    for(size_t i=0; i<arguments.size(); i++) {
        string package_data[3];
        if(!pkg.unpack_package_archive(arguments[i], (string)TMP_PATH + "/package" + to_string(i))) {
            out.errormsg("Can't extract package file:" + arguments[i] + ".");
            return 3;
        }
        if(!fs::is_directory((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO") ||
           !fs::is_directory((string)TMP_PATH + "/package" + to_string(i) + "/package/source") ||
           !fs::is_regular_file((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info")  ||
           !fs::is_regular_file((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/files") ||
           !pkg.read_package_data((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info", package_data)) {
            out.errormsg("Can't process " + arguments[i] + ": not a pako package.");
            return 4;
        }
    }
    for(size_t i=0; i<arguments.size(); i++) {
        string package_data[3];
        pkg.read_package_data((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info", package_data);
        if(db.isInDatabase(package_data[0])) {
            out.msg("Reinstalling " + package_data[0] + "...");
            remove({package_data[0], "--force-remove"});
        }
        if(!pkg.check_architecture(package_data[1])) {
            out.msg("Incompatible architecture detected for " + package_data[0] + ": " + package_data[1] + "\nContinue? [y/N] ");
            string ans;
            cin >> ans;
            if(!(ans == "y" || ans == "yes" || ans == "Y" || ans == "YES")) {
                out.msg("Cancelled.");
                return 5;
            }
        }
        
        if(!pkg.check_dependencies((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info") ||
           !pkg.check_conflicts((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info"))
           return 6;
    }
    /* Копируем контрольные файлы в VAR_PATH */
    for(size_t i=0; i<arguments.size(); i++) {
		string package_data[3];
        pkg.read_package_data((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info", package_data);

		try {
			for (const auto& entry : fs::recursive_directory_iterator((string)TMP_PATH + "/package" + std::to_string(i) + "/package/PAKO")) {
				if(entry.path().filename() == "files")
					continue;
				fs::copy(entry.path(), (string)VAR_PATH + "/control/" + package_data[0] + "." + entry.path().filename().string(), fs::copy_options::overwrite_existing);
			}
		}
		catch(const exception& e) {
		        out.errormsg("can't copy package control files.");
			out.debugmsg("CopyControlFiles:" + (string)e.what());
			return 7;
		}
        out.msg("Installing " + package_data[0] + ":" + package_data[1] + " " + "(" + package_data[2] + ")");
		if(fs::exists((string)VAR_PATH +"/control/" + package_data[0] + ".install"))
			system(("sh " + (string)VAR_PATH +"/control/" + package_data[0] + ".install" + " --preinst").c_str());
	}

	// Копируем содержимое всех распакованных пакетов в префикс
	for (size_t i = 0; i < arguments.size(); i++) {
        string package_data[3];
        pkg.read_package_data((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info", package_data);
        fstream filesList((string)VAR_PATH + "/control/" + package_data[0] + ".files", ios::app);
        for (const auto& entry : fs::recursive_directory_iterator((string)TMP_PATH + "/package" + to_string(i) + "/package/source")) {
            out.debugmsg(((string)TMP_PATH + "/package" + to_string(i) + "/package/source"));
            string path = entry.path().string().substr(((string)TMP_PATH + "/package" + to_string(i) + "/package/source").size());
            if(fs::is_directory(entry.path()) && !fs::is_symlink(entry.path()) && !fs::exists((string)PREFIX + "/" + path)) {
                out.debugmsg("Created directory: " + (string)PREFIX + "/" + path);
                fs::create_directories((string)PREFIX + "/" + path);
            } 
            else {
                try {
                    if(!fs::exists((string)PREFIX + "/" + path) && !fs::is_directory(entry.path())) {
                        filesList << path << "\n";
                        out.debugmsg("Creating file: " + (string)PREFIX + "/" + path);
                        out.debugmsg("Created file: " + (string)PREFIX + "/" + path);
                        fs::copy(entry.path(), (string)PREFIX + "/" + path, fs::copy_options::overwrite_existing | fs::copy_options::copy_symlinks);
                    } else if(fs::is_directory((string)PREFIX + "/" + path)) {
                        out.debugmsg("Creating Directory: " + (string)PREFIX + "/" + path);
                        fs::create_directories((string)PREFIX + "/" + path);
                        out.debugmsg("Created Directory: " + (string)PREFIX + "/" + path);
                    }
                }
                catch (std::filesystem::filesystem_error const& e) {
                    if (e.code().value() == 22) {
                        continue;
                    } else {
                        out.errormsg("Can't to copy package sources.");
                        out.debugmsg("Filesystem error message " + (string)e.what());
                        return 9;
                    }
                }
            }
        }
        filesList.close();
        out.debugmsg("Adding " + package_data[0] + " to database...");
        if(!db.addPackage(package_data, (string)VAR_PATH + "/control/" + package_data[0] + ".info", (string)VAR_PATH + "/control/" + package_data[0] + ".files")) {
            out.errormsg("can't add " + package_data[0] + " to database");
            return 10;
        }
        if(fs::exists((string)VAR_PATH +"/packages/" + package_data[0] + "/install"))
            system(("sh " + (string)VAR_PATH +"/packages/" + package_data[0] + "/install" + " --postinst").c_str());
    }
    pkg.clear_tmp();
    return 0;
}
