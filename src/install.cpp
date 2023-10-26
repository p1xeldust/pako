#include <archive.h>
#include <archive_entry.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <filesystem>
#include <shared_mutex>
#include <string>
#include <sstream>

#include "package/architectures.h"

using namespace std;
namespace fs = std::filesystem;

extern void debugmsg(std::string msg);
extern uint8_t Remove(std::vector<std::string> arguments);
bool mkTemp(fs::path tmpPath) {
    debugmsg("[mkTemp] Trying to create tmp dir " + tmpPath.string());
    try {
        if(fs::exists(tmpPath))
            fs::remove_all(tmpPath);
        fs::create_directories(tmpPath);
    } catch(const exception& e) {
        debugmsg("[DEBUG:mkTemp] Unable to create tmp dir. Exception: " + (string)e.what());
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
		cout << "Not opening " << ((fs::path)filePath).filename().string() << endl;
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
		debugmsg("[DEBUG_GPI]" + type + " " + value );
		#endif
	}
	#ifdef DEBUG_GPI
	for(uint8_t i = 0; i<3; i++) {
		if(packageData[i].size() <= 0)
			return false;
		debugmsg("[DEBUG_GPI] PackageData " + packageData[i]);
	}
	#endif
	dataFile.close();
	return true;
}

bool checkArch(string packageArch) {
	debugmsg("[Debug:Arch] Host: " + (string)HOSTARCH);
	debugmsg("[Debug:Arch] Package: " + packageArch);
	if((string)HOSTARCH != packageArch)
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
					debugmsg("[checkDeps] Dependency found: '" + dependency + "'");
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
					debugmsg("[checkDeps] Dependency solved: '" + dependencies[i] + "'");
					dependencies.erase(std::remove(dependencies.begin(), dependencies.end(), packageData[0]), dependencies.end());
					continue;
				}
		}
		for(const auto& entry : fs::directory_iterator((string)TMP_PATH)) {
			/* Проверка среди устанавливаемых */
        	getPackageData(entry.path().string() + "/package/PAKO/info", packageData);
			for(size_t i=0; i<dependencies.size(); ++i)
				if(dependencies[i] == packageData[0]) {
			    	debugmsg("[Debug:checkDeps] Dependency solved: '" + dependencies[i] + "'");
		            dependencies.erase(std::remove(dependencies.begin(), dependencies.end(), packageData[0]), dependencies.end());
					continue;
				}
		}
    }
    // Если пакеты остались в списке зависимостей, не установленные в системе или не готовые для установки, то пожаловаться на это.
	if(dependencies.size() > 0) {
		cout << "Package " << packageData[0] << " depends on";
		for(size_t i = 0; i < dependencies.size(); i++)
			cout << " '" << dependencies[i] << "'";
		cout << endl << "Install these package first.";
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
		cout << "Package " << packageData[0] << " conflicts with";
		for(size_t i=0; i<conflicts.size(); i++)
			cout << " '" << conflicts[i] << "'";
		cout << endl;
		return false;
	}
	return true;
}
int8_t Install(std::vector<std::string> arguments) {
    if(getuid() != 0) { // Проверочка на выполнение от суперпользователя.
        cerr << "Operation requires superuser privileges" << endl;
        return 127;
    }
	fs::remove_all((string)TMP_PATH);
	for(size_t i=0; i<arguments.size(); i++) {
		if(!mkTemp((string)TMP_PATH + "/package" + to_string(i))) {
			cout << "[Error] Unable to create temporary directory for package: " << arguments[i] << endl;
			return 1;
		}
		if(!unpack(arguments[i], (string)TMP_PATH + "/package" + to_string(i))) {
			cout << "[Error] Unable to extract package contents: " << arguments[i] << endl;
			return 2;
		} else 
			cout << "Extracting " << ((fs::path)arguments[i]).filename().string() << endl;
	}
	for(size_t i=0; i<arguments.size(); i++) { // Проверочки на совместимость каждого из пакетов в аргументах.
		string packageData[3];
		if(!getPackageData((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info", packageData)) { // Стандартная, на целостность контрольного файла пакета.
			cout << "[Error] Unable to process package " << arguments[i] << endl;
			return 3;
		}
		if(fs::exists((string)VAR_PATH + "/packages/" + packageData[0])) {
			cout << "Updating " << packageData[0] << ":" << packageData[1] << " " << "(" << packageData[2] << ")" << endl;
			Remove({packageData[0], "--force-remove"});
		} else
		    cout << "Preparing " << packageData[0] << ":" << packageData[1] << " " << "(" << packageData[2] << ")" << endl;
		/* Проверка архитектуры. */
		if(!checkArch(packageData[1])) {
			string ans;
			cout << "Uncompatible architecture detected:" << packageData[0] << endl << "Proceed? [y/N] ";
			cin >> ans;
			if(ans != "y" || ans != "yes")
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
			cerr << "[Error] Error while processing " << arguments[i] << endl;
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
			cerr << "[Error] Unable to extract package control files." << endl;
			debugmsg("[CopyControlFiles]" + (string)e.what());
			return 7;
		}
		cout << "Processing " << packageData[0] << ":" << packageData[1] << " " << "(" << packageData[2] << ")" << endl;
		if(fs::exists((string)VAR_PATH +"/packages/" + packageData[0] + "/install"))
			system(("sh " + (string)VAR_PATH +"/packages/" + packageData[0] + "/install" + " --preinst").c_str());		
	}
	// Копируем содержимое всех распакованных пакетов в префикс
	for(size_t i = 0; i<arguments.size(); i++) {
		string packageData[3];
		vector<string> deps;

		if(!getPackageData((string)TMP_PATH + "/package" + to_string(i) + "/package/PAKO/info", packageData)) {
			cerr << "[Error] Error while processing " << arguments[i] << endl;
			return 4;
		}

		cout << "Installing " << packageData[0] << ":" << packageData[1] << " " << "(" << packageData[2] << ")" << endl;
	}
	for (size_t i = 0; i < arguments.size(); i++) {
		string packageData[3];
		std::string packageDatapackageData[3];
		getPackageData((std::string)TMP_PATH + "/package" + std::to_string(i) + "/package/PAKO/info", packageData);
		fstream filesList((std::string)VAR_PATH + "/packages/" + packageData[0] + "/files", ios::app);
		fs::copy((std::string)TMP_PATH + "/package" + std::to_string(i) + "/package/PAKO", (std::string)VAR_PATH + "/packages/" + packageData[0], fs::copy_options::overwrite_existing);

		for (const auto& entry : fs::recursive_directory_iterator((std::string)TMP_PATH + "/package" + std::to_string(i) + "/package/source")) {
			std::string path = entry.path().string().substr(((std::string)TMP_PATH + "/package" + std::to_string(i) + "/package/source").size());





			// ПОФИКСИТЬ





			try {
				if(!fs::exists((std::string)PREFIX + "/" + path) && !fs::is_directory(entry.path())) {
					filesList << path << "\n";
					debugmsg("Creating file: " + (std::string)PREFIX + "/" + path);	
					debugmsg("Created file: " + (std::string)PREFIX + "/" + path);
					fs::copy(entry.path(), (std::string)PREFIX + "/" + path, fs::copy_options::overwrite_existing | fs::copy_options::copy_symlinks);
				} else if(fs::is_directory((std::string)PREFIX + "/" + path)) {
					debugmsg("Creating Directory: " + (std::string)PREFIX + "/" + path);
					fs::create_directories((string)PREFIX + "/" + path);
					debugmsg("Created Directory: " + (std::string)PREFIX + "/" + path);
				}
			} catch (std::filesystem::filesystem_error const& e) {
				if (e.code().value() == 22) {
					continue;
				} else {
					std::cerr << "[Error] Unable to extract package source files." << std::endl;
					debugmsg("Filesystem error message " + (string)e.what());
					return 8;
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
