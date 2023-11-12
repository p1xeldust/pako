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

#include "output/print.h"

extern int8_t Remove(std::vector<std::string> arguments);

using namespace std;
namespace fs = std::filesystem;

extern Print out;

bool mkTemp(fs::path tmpPath) {
    out.debugmsg("Trying to make temp dir: " + tmpPath.string());
    try {
        if(fs::exists(tmpPath))
            fs::remove_all(tmpPath);
        fs::create_directories(tmpPath);
    } catch(const exception& e) {
        out.debugmsg("[DEBUG:mkTemp] Unable to create tmp dir. Exception: " + (string)e.what());
        return false;
    }
    return true;
}


bool unpack(string filePath, string destinationPath) {
struct archive* a;
struct archive_entry* entry;

a = archive_read_new();
archive_read_support_filter_xz(a);
archive_read_support_format_tar(a);

	if(archive_read_open_filename(a, filePath.c_str(), 10240) != 0) {
		archive_read_free(a);
		out.errormsg("Not opening " + ((fs::path)filePath).filename().string());
		return false;
	}

	while(archive_read_next_header(a, &entry) == 0) {
		const char* entryPath = archive_entry_pathname(entry);
		string extractPath = destinationPath + "/" + entryPath;

		// "Распаковываем" символическую ссылку.
		if(archive_entry_filetype(entry) == AE_IFLNK) {
			if (symlink(archive_entry_symlink(entry), extractPath.c_str()) != 0) {
				// Ну или нет, если есть какая-то проблема
				archive_read_close(a);
				archive_read_free(a);
				return false;
			}
			continue;
		}
		// "Распаковываем" папку (тупо создаём эту папку).
		if(archive_entry_filetype(entry) == AE_IFDIR) {
			if (mkdir(extractPath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
				// Ну или нет, если есть какая-то проблема
				archive_read_close(a);
				archive_read_free(a);
				return false;
			}
				continue;
			}

			FILE* file = fopen(extractPath.c_str(), "wb");

			if(!file) {
				archive_read_close(a);
				archive_read_free(a);
				return false;
			}

			const void* buff;
			size_t size;
			la_int64_t offset;

			while(archive_read_data_block(a, &buff, &size, &offset) == 0)
				fwrite(buff, 1, size, file);

			fclose(file);

			// Сохраняем права доступа файла.
			mode_t mode = archive_entry_perm(entry);
			chmod(extractPath.c_str(), mode);
		}

	archive_read_close(a);
	archive_read_free(a);
	return true;

}

bool getPackageData(string dataFilePath, string packageData[3]) {
    ifstream dataFile(dataFilePath);
    string type, value;
    for(string line; getline(dataFile, line); dataFile.good()) {
        type = line.substr(0, line.find_first_of(": "));
        value = line.substr(line.find_last_of(" ")+1);
        if(type == "name")
            packageData[0] = value;
        else if(type == "version")
            packageData[2] = value;
        else if(type == "arch")
            packageData[1] = value;
		#ifdef DEBUG_GPI
		out.debugmsg("[DEBUG_GPI]" + type + " " + value);
		#endif
	}
	#ifdef DEBUG_GPI
	for(uint8_t i = 0; i<3; i++) {
		if(packageData[i].size() <= 0)
			return false;
		out.debugmsg("[DEBUG_GPI] PackageData " + packageData[i]);
	}
	#endif
	dataFile.close();
	return true;
}

bool checkArch(string packageArch) {
	struct utsname hostData;
	uname(&hostData);
	out.debugmsg("[Debug:Arch] Host: " + (string)hostData.machine);
	out.debugmsg("[Debug:Arch] Package: " + packageArch);
	if(hostData.machine != packageArch)
		return false;
	return true;
}

int8_t checkVersion(string installedVersion, string installingVersion) {

    istringstream version1(installedVersion);
    istringstream version2(installingVersion);

    uint8_t v1num, v2num;
    char delimiter;

    while (version1 || version2) {
        if ((version1 >> v1num) && (version2 >> v2num)) {
            if (v1num < v2num) return 0;
            else if (v1num > v2num) return 1;
            }
        else {
            if(version1) return 0;
            if(version2) return 1;
        }

        if ((version1 >> delimiter) && delimiter != '.' || (version2 >> delimiter) && delimiter != '.') {
            return -1; // Если разделители не совпадают, версии разные. Думаю добавить ещё одну ошибку о результате
        }
    }
    return -1;
}
bool checkDeps(string dataFilePath) {
	vector<string> dependencies;
	string packageData[3];
	ifstream dataFile(dataFilePath, ios::in);
	for(string line; getline(dataFile, line); dataFile.good())
		if(line.find("deps:") != std::string::npos) {
			istringstream dependenciesList(line.substr(line.find(':') + 1).erase(0, line.find_first_not_of(" \t")));
			string dependency;
			for(string dependency; dependenciesList >> dependency;)
				if(dependency.size() > 0) {
					out.debugmsg("[checkDeps] Dependency found for " + packageData[0] + "'" + dependency + "'");
					dependencies.push_back(std::move(dependency));
				}
		}
    /* Проверка на установленные пакеты */
	if(dependencies.size() > 0) {
		/* Проверка среди установленных */
		for(const auto& entry : fs::directory_iterator((string)VAR_PATH + "/packages/")) {
			getPackageData(entry.path().string() + "/info", packageData);
			for(size_t i=0; i<dependencies.size(); ++i)
				if(dependencies[i] == packageData[0]) {
					out.debugmsg("[checkDeps] Dependency solved for " + packageData[0] + " '" + dependencies[i] + "'");
					dependencies.erase(std::remove(dependencies.begin(), dependencies.end(), packageData[0]), dependencies.end());
					continue;
				}
		}
		for(const auto& entry : fs::directory_iterator((string)TMP_PATH)) {
			/* Проверка среди устанавливаемых */
        	getPackageData(entry.path().string() + "/package/PAKO/info", packageData);
			for(size_t i=0; i<dependencies.size(); ++i)
				if(dependencies[i] == packageData[0]) {
			    	out.debugmsg("[Debug:checkDeps] Dependency solved: '" + dependencies[i] + "'");
		            dependencies.erase(std::remove(dependencies.begin(), dependencies.end(), packageData[0]), dependencies.end());
					continue;
				}
		}
    }
    // Если пакеты остались в списке зависимостей, не установленные в системе или не готовые для установки, то пожаловаться на это.
	if(dependencies.size() > 0) {
		out.msg("Package " + packageData[0] + " depends on:");
		for(size_t i = 0; i < dependencies.size(); i++)
		    cout << " \e[5;1m'" + dependencies[i] + "'\e[1;0m";
		cout << endl;
		out.msg("Install these package first.");
		return false;
	}
	return true;
}
bool checkConflicts(string dataFilePath) {
	vector<string> conflicts;
	string packageData[3];
	ifstream dataFile(dataFilePath, ios::in);
	getPackageData(dataFilePath, packageData);
	for(string line; getline(dataFile, line); dataFile.good()) {
		if(line.find("conflicts:") != std::string::npos) {
			istringstream conflictsList(line.substr(line.find(':') + 1).erase(0, line.find_first_not_of(" \t")));
			string conflict;
			for(string conflict; conflictsList >> conflict;) {
				for(const auto& entry : fs::directory_iterator((string)TMP_PATH)) {
					getPackageData(entry.path().string() + "/package/PAKO/info", packageData);

					if(conflict == packageData[0])
						conflicts.push_back(std::move(conflict));
				}

				for(const auto& entry : fs::directory_iterator((string)VAR_PATH + "/packages/")) {
					getPackageData(entry.path().string() + "/info", packageData);

					if(conflict == packageData[0])
						conflicts.push_back(std::move(conflict));
				}
			}
		}
	}
	if(conflicts.size() > 0) {
		getPackageData(dataFilePath, packageData);
		out.msg(("Package " + packageData[0] + " conflicts with").c_str());
		for(size_t i=0; i<conflicts.size(); i++)
			cout << " \e[5;1m'" << conflicts[i] << "'\e[1;0m";
		cout << endl;
		return false;
	}
	return true;
}
int8_t Install(std::vector<std::string> arguments) {
    #if NOSU == 0
    if(getuid() != 0) { // Проверочка на выполнение от суперпользователя.
        out.errormsg("Operation requires superuser privileges");
        return 127;
    }
    #endif
    for(size_t i=0; i<arguments.size(); i++) {
        if(!fs::is_regular_file(arguments[i])) {
            out.errormsg((arguments[i] + " is not a file.").c_str());
            return 1;
        }
     }
	fs::remove_all((string)TMP_PATH);
	for(size_t i=0; i<arguments.size(); i++) {
		if(!mkTemp((string)TMP_PATH + "/package" + to_string(i))) {
			out.errormsg("Unable to create temporary directory for package: " + arguments[i]);
			return 1;
		}
		if(!unpack(arguments[i], (string)TMP_PATH + "/package" + to_string(i))) {
			out.errormsg("Unable to extract package contents: " + arguments[i]);
			return 2;
		} else
			out.msg("Extracting " + ((fs::path)arguments[i]).filename().string());
	}
	for(size_t i=0; i<arguments.size(); i++) { // Проверочки на совместимость каждого из пакетов в аргументах.
		string packageData[3];
		if(!getPackageData((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info", packageData)) { // Стандартная, на целостность контрольного файла пакета.
			out.errormsg("Unable to process package " + arguments[i]);
			return 3;
		}
		if(fs::exists((string)VAR_PATH + "/packages/" + packageData[0])) {
			out.msg("Updating " + packageData[0] + ":" + packageData[1] + " " + "(" + packageData[2] + ")");
			Remove({packageData[0], "--force-remove"});
		} else
		    out.msg("Preparing " + packageData[0] + ":" + packageData[1] + " " + "(" + packageData[2] + ")");
		/* Проверка архитектуры. */
		if(!checkArch(packageData[1])) {
			string ans;
			out.msg("Incompatible architecture: " + packageData[0] + " - " + packageData[1] + "\n" + "Proceed? [y/N] ");
			cin >> ans;
			if(!(ans == "y" || ans == "yes"))
				return 5;
		}
		/* Зависимости и конфликты */
		if(!checkDeps((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info"))
			return 6;
		if(!checkConflicts((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info"))
			return 7;
	}
	/* Раз ошибок ни в каком из пакетов не имеется, устанавливаем все начиная с контрольных файлов */
	for(size_t i=0; i<arguments.size(); i++) {
		string tmpDataFilePath = (string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info";
		vector<string> deps;
		string packageData[3];
		if(!getPackageData(tmpDataFilePath, packageData)) {
			out.errormsg(("Can't process " + arguments[i]).c_str());
			return 4;
		}
		try {
			fs::create_directories("/var/lib/pako/packages/" + packageData[0]);
			for (const auto& entry : fs::recursive_directory_iterator((std::string)TMP_PATH + "/package" + std::to_string(i) + "/package/PAKO")) {
				if(entry.path().filename().string() == "files")
					continue;
				fs::create_directories((string)VAR_PATH + "/packages/" + packageData[0]);
				fs::copy(entry.path().string(), (string)VAR_PATH + "/packages/" + packageData[0], fs::copy_options::overwrite_existing);
			}
		}

		catch(const exception& e) {
		        out.errormsg("[Error] Unable to extract package control files.");
			out.debugmsg(("[CopyControlFiles]" + (string)e.what()).c_str());
			return 7;
		}
                out.msg("Installing " + packageData[0] + ":" + packageData[1] + " " + "(" + packageData[2] + ")");
		if(fs::exists((string)VAR_PATH +"/packages/" + packageData[0] + "/install"))
			system(("sh " + (string)VAR_PATH +"/packages/" + packageData[0] + "/install" + " --preinst").c_str());
	}
	// Копируем содержимое всех распакованных пакетов в префикс
	for(size_t i = 0; i<arguments.size(); i++) {
		string packageData[3];
		vector<string> deps;

		if(!getPackageData((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info", packageData)) {
			out.errormsg("Can't process " + arguments[i]);
			return 4;
		}

		out.msg("Installing " + packageData[0] + ":" + packageData[1] + " " + "(" +packageData[2] + ")");
	}
	for (size_t i = 0; i < arguments.size(); i++) {
	string packageData[3];
	getPackageData((std::string)TMP_PATH + "/package" + std::to_string(i) + "/package/PAKO/info", packageData);
	fstream filesList((std::string)VAR_PATH + "/packages/" + packageData[0] + "/files", ios::app);
	fs::copy((std::string)TMP_PATH + "/package" + std::to_string(i) + "/package/PAKO", (std::string)VAR_PATH + "/packages/" + packageData[0], fs::copy_options::overwrite_existing);
            for (const auto& entry : fs::recursive_directory_iterator((std::string)TMP_PATH + "/package" + std::to_string(i) + "/package/source")) {
		std::string path = entry.path().string().substr(((std::string)TMP_PATH + "/package" + std::to_string(i) + "/package/source").size());
		if (fs::is_directory(entry.path()) && !fs::is_symlink(entry.path()) && !fs::exists((std::string)PREFIX + "/" + path)) {
		    out.debugmsg("Created directory: " + (std::string)PREFIX + "/" + path);
		    fs::create_directories((std::string)PREFIX + "/" + path);
		} else {
		    try {
			if(!fs::exists((std::string)PREFIX + "/" + path) && !fs::is_directory(entry.path())) {
			    filesList << path << "\n";
			    out.debugmsg("Creating file: " + (std::string)PREFIX + "/" + path);
			    out.debugmsg("Created file: " + (std::string)PREFIX + "/" + path);
			    fs::copy(entry.path(), (std::string)PREFIX + "/" + path, fs::copy_options::overwrite_existing | fs::copy_options::copy_symlinks);
			} else if(fs::is_directory((std::string)PREFIX + "/" + path)) {
			    out.debugmsg("Creating Directory: " + (std::string)PREFIX + "/" + path);
			    fs::create_directories((string)PREFIX + "/" + path);
			    out.debugmsg("Created Directory: " + (std::string)PREFIX + "/" + path);
			}
		    } catch (std::filesystem::filesystem_error const& e) {
			if (e.code().value() == 22) {
			    continue;
			} else {
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
	}
	#if DEMO == 1
		std::cout << "That's how it works!" << std::endl;
	#endif
	return 0;
}
