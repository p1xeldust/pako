#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include "unpack.cpp"
#include <filesystem>
#include <stdlib.h>
#include "constants.hpp"

using namespace std;

namespace fs = std::filesystem;

int check_architecture(std::string package_arch) {
	if(HOST_ARCH != package_arch) {
		return 1;
	}
	return 0;
}

int install(int argc, char* argv[]) {
	if(geteuid() != 0) {
		printf("[\e[31mE\e[39m] Operation requires superuser privilegies\n");
		return 127;
	}
	for(int i=2; i<argc; i++) {
		if(fs::is_directory(constants::TMP_PATH)) {
			fs::remove_all(constants::TMP_PATH);
		}
		try {
			fs::create_directory(constants::TMP_PATH);
		} catch(const std::exception& e) {
			printf("[\e[31mE\e[39m] Unable to create temporary directory.\n");
			return(1);
		}		
		// Копирование пакета в tmp
		std::ifstream package_file(argv[i]);
		std::ofstream package_tmp(constants::TMP_PATH + "/package.tmp");
		package_tmp << package_file.rdbuf();
		package_file.close();
		
		// Распаковочка пакета
		printf("Unpacking %s...\n", argv[i]);
		if(unpack((constants::TMP_PATH + "/package.tmp").c_str(),constants::TMP_PATH)) {
			printf("[\e[31mE\e[39m] Error occured while unpacking %s: Bad package.\n", argv[i]);
			return(1);
		}
		package_tmp.close();
		fs::remove(constants::TMP_PATH + "/package.tmp"); // Удаление пакета из tmp
		
		std::ifstream infofile(constants::TMP_PATH + "/package/PAKO/info"); // Проверяем наличие info файла в пакете. Если есть - продолжаем
		if(!infofile.good()) {
			printf("[\e[31mE\e[39m] Error occured while unpacking %s: Is not a package.\n", argv[i]);
			return(1);
		}
		// Получения имени, версии и архитектуры пакета
		std::string package_name, package_version, package_arch;
		for(std::string line; getline(infofile, line, ' ');) {
			if (line == "name:") {
				getline(infofile, line);
				package_name = line;
				if(!(package_name.size() > 0)) {
					printf("[\e[31mE\e[39m] Error occured while installing %s: Unable to detect package name", argv[i]);
					return 14;
				}
			}
			if (line == "version:") {
				getline(infofile, line);
				package_version = line;
				if(!(package_version.size() > 0)) {
					printf("[\e[31mE\e[39m] Error occured while installing %s: Unable to detect package version", argv[i]);
					return 15;
				}
			}
			if (line == "arch:") {
				getline(infofile, line);
				package_arch = line;
				if(!(package_arch.size() > 0)) {
					printf("[\e[31mE\e[39m] Error occured while installing %s: Unable to detect package architecture", argv[i]);
					return 16;
				}
			}
		}
		
		std::cout << "Preparing to install " << package_name << ":" << package_arch << " v" << package_version << std::endl; 
		
		// Проверка архитектуры пакета
		if(check_architecture(package_arch)) {
			printf("[\e[31mE\e[39m] Error occured while installing %s:%s v%s:", package_name.c_str(), package_arch.c_str(), package_version.c_str());
			return 2;
		}
		try {
			if(fs::exists(constants::VAR_PATH + "/packages/" + package_name))
				fs::remove_all(constants::VAR_PATH + "/packages/" + package_name);
			fs::create_directories(constants::VAR_PATH + "/packages/" + package_name);
			fs::copy(constants::TMP_PATH + "/package/PAKO/files", constants::VAR_PATH + "/packages/" + package_name + "/files");
			fs::copy(constants::TMP_PATH + "/package/PAKO/info", constants::VAR_PATH + "/packages/" + package_name + "/info");

			if(fs::exists(constants::TMP_PATH + "/package/PAKO/install")) {
				fs::copy(constants::TMP_PATH + "/package/PAKO/install", constants::VAR_PATH + "/packages/" + package_name + "/install");
				if(fs::exists(constants::VAR_PATH + "/packages/" + package_name + "/install")) {
					printf("Running intallation configuration for %s:%s v%s\n:", package_name.c_str(), package_arch.c_str(), package_version.c_str());
					system(("sh " + constants::VAR_PATH + "/packages/" + package_name + "/install --preinstall").c_str());
				}
			}
		} catch(const exception& e) {
			printf("[\e[31mE\e[39m] Error occured while installing %s:%s v%s:", package_name.c_str(), package_arch.c_str(), package_version.c_str());
			std::cout << e.what() << std::endl;
			return 2;
		}

		std::ifstream files_list(constants::VAR_PATH + "/packages/" + package_name + "/files");
		std::string filepath;

		while(getline(files_list, filepath)) {

			if(fs::is_directory(constants::TMP_PATH + "/package/source/" + filepath)) {
				fs::create_directory(constants::PREFIX + filepath); 
			} else {
				if(fs::exists(constants::PREFIX + filepath))
					fs::remove(constants::PREFIX + filepath);
				std::ifstream source_file(constants::TMP_PATH + "/package/source/" + filepath, std::ios::binary);
				std::ofstream destination_file(constants::PREFIX + filepath, std::ios::binary);
				destination_file << source_file.rdbuf();
				if (access(filepath.c_str(), X_OK) == 0)
					fs::permissions((constants::PREFIX + filepath).c_str(), fs::perms::owner_exec | fs::perms::others_exec, fs::perm_options::add);
				source_file.close();
				destination_file.close();
			}
		}
		files_list.close();
		ofstream status_file(constants::VAR_PATH + "status",std::ios::ate);
		status_file << infofile.rdbuf();
		infofile.close();

		if(fs::exists(constants::VAR_PATH + "/packages/" + package_name + "/install")) {
			printf("Running intallation configuration for %s:%s v%s\n:", package_name.c_str(), package_arch.c_str(), package_version.c_str());
			system(("sh " + constants::VAR_PATH + "/packages/" + package_name + "/install --postinstall").c_str());
		}
		printf("Installed %s:%s v%s\n", package_name.c_str(), package_arch.c_str(), package_version.c_str());
	}
	return 0;
}
