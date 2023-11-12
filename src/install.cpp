#include <archive.h>
#include <archive_entry.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sys/utsname.h>
#include <cstring>
#include <filesystem>
#include <shared_mutex>
#include <string>
#include <sstream>

#include "package/package.h"
#include "pako.h"

using namespace std;
namespace fs = std::filesystem;

std::string packageData[3];

int8_t Pako::install(std::vector<std::string> arguments) {
    #if NOSU == 0
    if(getuid() != 0) { // Проверочка на выполнение от суперпользователя.
        out.errormsg("Operation requires superuser privileges");
        return 127;
    }
    #endif
    for(size_t i=0; i<arguments.size(); i++) {
        if(!fs::is_regular_file(arguments[i])) {
            out.errormsg(arguments[i] + " is not a file.");
            return 1;
        }
    }
    fs::remove_all((string)TMP_PATH);
    for(size_t i=0; i<arguments.size(); i++) {
        if(!pkg.mkTemp((string)TMP_PATH + "/package" + to_string(i))) {
            out.errormsg("Unable to create temporary directory for package: " + arguments[i]);
            return 1;
        }
        out.msg("Extracting " + ((fs::path)arguments[i]).filename().string());
        if(!pkg.unpack(arguments[i], (string)TMP_PATH + "/package" + to_string(i))) {
            out.errormsg("Unable to extract package contents: " + arguments[i]);
            return 2;
	}
        if(
    }
    // Порверки на совместимость устанавливаемых пакетов
    for(size_t i=0; i<arguments.size(); i++) {
        if(!pkg.readPackageData((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info", packageData)) { // Стандартная, на целостность контрольного файла пакета.
            out.errormsg("Unable to process package " + arguments[i]);
            return 3;
        }
        if(fs::exists((string)VAR_PATH + "/packages/" + packageData[0] + ".info") || db.isInDatabase((std::string)VAR_PATH + "/packages.db", packageData[0]) {
            out.msg("Updating " + packageData[0] + ":" + packageData[1] + " " + "(" + packageData[2] + ")");
            //Remove({packageData[0], "--force-remove"});
        }
        else
            out.msg("Preparing " + packageData[0] + ":" + packageData[1] + " " + "(" + packageData[2] + ")");
        /* Проверка архитектуры. */
        if(!pkg.checkArch(packageData[1])) {
            string ans;
            out.msg("Incompatible architecture: " + packageData[0] + " - " + packageData[1]);
            cout << "Proceed? [y/N] ";
            cin >> ans;
            if(!(ans == "y" || ans == "yes"))
	        return 5;
        }
        /* Зависимости и конфликты */
        if(!pkg.checkDeps((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info"))
            return 6;
        if(!pkg.checkConflicts((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info"))
            return 7;
    }
    /* Раз ошибок ни в каком из пакетов не имеется, устанавливаем все начиная с контрольных файлов */
    for(size_t i=0; i<arguments.size(); i++) {
        try {
            pkg.readPackageData((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info", packageData);
            if(!fs::is_regular_file((std::string)VAR_PATH + "/packages.db"))
                db.initDatabase((std::string)VAR_PATH + "/packages.db");
            db.addPackage((std::string)VAR_PATH + "/packages.db", packageData, (string)VAR_PATH + "/packages/" + packageData[0] + ".list", (string)VAR_PATH + "/packages/" + packageData[0] + ".info");
            fs::create_directories((std::string)VAR_PATH + "/packages/");
            for (const auto& entry : fs::recursive_directory_iterator((std::string)TMP_PATH + "/package" + std::to_string(i) + "/package/PAKO")) {
                if(entry.path().filename().string() == "files")
                    continue;
                fs::copy(entry.path().string(), (string)VAR_PATH + "/packages/" + packageData[0] + "." + entry.path().filename().string(), fs::copy_options::overwrite_existing);
            }
            out.msg("Configuring " + packageData[0] + ":" + packageData[1] + " " + "(" + packageData[2] + ")");
            if(fs::exists((string)VAR_PATH +"/packages/" + packageData[0] + "/install"))
                system(("sh " + (string)VAR_PATH +"/packages/" + packageData[0] + "/install" + " --preinst").c_str());
        }
        catch(const exception& e) {
            out.errormsg("[Error] Unable to extract package control files.");
            out.debugmsg("[CopyControlFiles]" + (string)e.what());
            return 7;
        }
    }
    // Копируем содержимое всех распакованных пакетов в префикс
    for(size_t i = 0; i < arguments.size(); i++) {
        pkg.readPackageData((std::string)TMP_PATH + "/package" + std::to_string(i) + "/package/PAKO/info", packageData);
        fstream filesList((std::string)VAR_PATH + "/packages/" + packageData[0] + "/files", ios::app);
        fs::copy((std::string)TMP_PATH + "/package" + std::to_string(i) + "/package/PAKO", (std::string)VAR_PATH + "/packages/" + packageData[0], fs::copy_options::overwrite_existing);
        for(const auto& entry : fs::recursive_directory_iterator((std::string)TMP_PATH + "/package" + std::to_string(i) + "/package/source")) {
            std::string path = entry.path().string().substr(((std::string)TMP_PATH + "/package" + std::to_string(i) + "/package/source").size());
            if (fs::is_directory(entry.path()) && !fs::is_symlink(entry.path()) && !fs::exists((std::string)PREFIX + "/" + path)) {
                out.debugmsg("Created directory: " + (std::string)PREFIX + "/" + path);
                fs::create_directories((std::string)PREFIX + "/" + path);
            }
            else {
                try {
                    if(!fs::exists((std::string)PREFIX + "/" + path) && !fs::is_directory(entry.path())) {
                        filesList << path << "\n";
                        fs::copy(entry.path(), (std::string)PREFIX + "/" + path, fs::copy_options::overwrite_existing | fs::copy_options::copy_symlinks);
                        out.debugmsg("Created file: " + (std::string)PREFIX + "/" + path);
                    }
                    else if(fs::is_directory((std::string)PREFIX + "/" + path)) {
                        fs::create_directories((string)PREFIX + "/" + path);
                        out.debugmsg("Created Directory: " + (std::string)PREFIX + "/" + path);
                    }
                }
                catch (std::filesystem::filesystem_error const& e) {
                    if (e.code().value() == 22)
                        continue;
                    else {
                        out.errormsg("[Error] Unable to extract package source files.");
                        out.debugmsg("Filesystem error message " + (string)e.what());
                        return 8;
                    }
                }
            }
        }
        filesList.close();
        if(fs::exists((string)VAR_PATH +"/packages/" + packageData[0] + "/install"))
            system(("sh " + (string)VAR_PATH +"/packages/" + packageData[0] + "/install" + " --postinst").c_str());
        out.msg("Installed " + packageData[0] + ":" + packageData[1] + " " + "(" +packageData[2] + ")");
    }
    return 0;
}
