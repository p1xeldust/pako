#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <filesystem>
#include <stdlib.h>

#include "unpack.cpp"
#include "constants.h"

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
		std::string package_name, package_version, package_arch;
		std::string package_info[3];
		//	Манипуляции с tmp
		
		if(fs::is_directory(constants::TMP_PATH))
			fs::remove_all(constants::TMP_PATH);
		
		try {
			fs::create_directory(constants::TMP_PATH);
		} catch(const std::exception& e) {
			printf("[\e[31mE\e[39m] Unable to create temporary directory.\n");
			return 1;
		}		

		// Копирование пакета в tmp

		fs::copy(std::filesystem::current_path().string() + "/" + argv[i], constants::TMP_PATH + "/package.tmp");

		// Распаковочка пакета

		printf("Unpacking %s...\n", argv[i]);
		if(!unpack((constants::TMP_PATH + "/package.tmp").c_str(),constants::TMP_PATH.c_str())) {
			printf("[\e[31mE\e[39m] %s: Bad package.\n", argv[i]);
			return(1);
		}
		
		std::ifstream infofile(constants::TMP_PATH + "/package/PAKO/info"); // Проверяем наличие info файла в пакете. Если есть - продолжаем
		if(!infofile.good()) {
			printf("[\e[31mE\e[39m] %s: Is not a package.\n", argv[i]);
			return(1);
		}

		// Получения имени, версии и архитектуры пакета

		for(std::string line; getline(infofile, line, ' ');) {
			if (line == "name:") {
				getline(infofile, line);
				package_name = line;
				if(!(package_name.size() > 0)) {
					printf("[\e[31mE\e[39m] %s: Unable to detect package name", argv[i]);
					return 14;
				}
			}
			if (line == "version:") {
				getline(infofile, line);
				package_version = line;
				if(!(package_version.size() > 0)) {
					printf("[\e[31mE\e[39m] %s: Unable to detect package version", argv[i]);
					return 15;
				}
			}
			if (line == "arch:") {
				getline(infofile, line);
				package_arch = line;
				if(!(package_arch.size() > 0)) {
					printf("[\e[31mE\e[39m] %s: Unable to detect package architecture", argv[i]);
					return 16;
				}
			}
		}
		infofile.close();		
		
		printf("Installing %s:%s v%s\n", package_name.c_str(), package_arch.c_str(), package_version.c_str());

		
		//	Проверка архитектуры пакета
		if(check_architecture(package_arch)) {
			printf("\n[\e[31mE\e[39m] %s:%s v%s: Incompatible architecture\n", package_name.c_str(), package_arch.c_str(), package_version.c_str());
			return 2;
		}
		//	Копируем информационные файлы пакета в VAR_PATH
		try {
			std::string VAR_PKG = constants::VAR_PATH + "/packages/" + package_name;
			if(fs::exists(constants::VAR_PATH + "/packages/" + package_name)) {
				printf("[\e[31mE\e[39m] %s:%s v%s: Package is already installed\n", package_name.c_str(), package_arch.c_str(), package_version.c_str());
				return 1;
			}
			fs::create_directories(VAR_PKG);
			fs::copy(constants::TMP_PATH + "/package.tmp", VAR_PKG + "/" + package_name + ".pako");
			fs::remove(constants::TMP_PATH + "/package.tmp");
			fs::copy(constants::TMP_PATH + "/package/PAKO/files", VAR_PKG + "/files");
			fs::copy(constants::TMP_PATH + "/package/PAKO/info", VAR_PKG + "/info");

			if(fs::exists(constants::TMP_PATH + "/package/PAKO/install")) {
				fs::copy(constants::TMP_PATH + "/package/PAKO/install", constants::VAR_PATH + "/packages/" + package_name + "/install");
			}
		} catch(const std::exception& e) {
			printf("[\e[31mE\e[39m] %s:%s v%s: Unable to copy package information files\n", package_name.c_str(), package_arch.c_str(), package_version.c_str());
			std::cout << e.what() << std::endl;
			return 2;
		}
		
		// Выполняем пре-конфигурацию пакета через shell скрипт install
		
		if(fs::exists(constants::VAR_PATH + "/packages/" + package_name + "/install")) {
			printf("Configuring %s:%s v%s\n", package_name.c_str(), package_arch.c_str(), package_version.c_str());
			system(("sh " + constants::VAR_PATH + "/packages/" + package_name + "/install --preinstall").c_str());
		}
		
		// Копируем файлы внутри пакета в корень
		
		try {
			for (const auto& entry : fs::directory_iterator(constants::TMP_PATH + "/package/source/")) {
				const std::string entryPath = entry.path().string();
				const std::string entryName = entry.path().filename().string();
				const std::string destinationEntryPath = constants::PREFIX + "/" + entryName;
				fs::copy(entryPath, destinationEntryPath, fs::copy_options::recursive | fs::copy_options::copy_symlinks);
			}
		} catch(const std::exception& e) {

		}
		if(fs::exists(constants::VAR_PATH + "/packages/" + package_name + "/install")) {
			printf("Setting up %s:%s v%s\n", package_name.c_str(), package_arch.c_str(), package_version.c_str());
			system(("sh " + constants::VAR_PATH + "/packages/" + package_name + "/install --postinstall").c_str());
		}
		printf("Installed %s:%s v%s\n",package_name.c_str(), package_arch.c_str(), package_version.c_str());
	}
	return 0;
}
