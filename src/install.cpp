#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include "unpack.cpp"
#include <filesystem>

#ifdef __x86_64__
#define HOST_ARCH "amd64"
#endif
#ifdef __x86__
#define HOST_ARCH "i386"
#endif
#ifdef __arm__
#define HOST_ARCH "armhf"
#endif
#ifdef __arm64__
#define HOST_ARCH "aarch64"
#endif

std::string VAR_PATH = "/var/lib/pako";
std::string PREFIX = "/opt/";

namespace fs = std::filesystem;

int check_architecture(std::string package_arch) {
	if(HOST_ARCH != package_arch) {
		return 1;
	}
	return 0;
}

void install(int argc, char* argv[]) {
	for(int i=2; i<argc; i++) {
		if(fs::is_directory("/tmp/pako")) {
			fs::remove_all("/tmp/pako");
		}
		if(!fs::create_directory("/tmp/pako")) {
			printf("[\e[31mE\e[39m] Unable to create temporary directory.\n");
			exit(1);
		}		
		// Копирование пакета в tmp
		std::ifstream package_file(argv[i]);
		std::ofstream package_tmp("/tmp/pako/package.tmp");
		package_tmp << package_file.rdbuf();
		package_file.close();
		
		// Распаковочка пакета
		printf("Unpacking %s...\n", argv[i]);
		if(!unpack("/tmp/pako/package.tmp")) {
			printf("[\e[31mE\e[39m] Error occured while unpacking %s: Bad package.\n", argv[i]);
			exit(1);
		}
		package_tmp.close();
		remove("/tmp/pako/package.tmp"); // Удаление пакета из tmp
		
		std::ifstream infofile("/tmp/pako/package/PAKO/info"); // Проверяем наличие info файла в пакете. Если есть - продолжаем
		if(!infofile.good()) {
			printf("[\e[31mE\e[39m] Error occured while unpacking %s: Is not a package.\n", argv[i]);
			exit(1);
		}
		// Получения имени, версии и архитектуры пакета
		std::string package_name, package_version, package_arch;
		for(std::string line; getline(infofile, line, ' ');) {
			if (line == "name:") {
				getline(infofile, line);
				package_name = line;
				std::cout << package_name << std::endl;
				if(!(package_name.size() > 0)) {
					printf("[\e[31mE\e[39m] Error occured while installing %s: Unable to detect package name", argv[i]);
					exit(14);
				}
			}
			if (line == "version:") {
				getline(infofile, line);
				package_version = line;
				std::cout << package_version << std::endl;
				if(!(package_version.size() > 0)) {
					printf("[\e[31mE\e[39m] Error occured while installing %s: Unable to detect package version", argv[i]);
					exit(15);
				}
			}
			if (line == "arch:") {
				getline(infofile, line);
				package_arch = line;
				std::cout << package_arch << std::endl;
				if(!(package_arch.size() > 0)) {
					printf("[\e[31mE\e[39m] Error occured while installing %s: Unable to detect package architecture", argv[i]);
					exit(16);
				}
			}
		}
		
		std::cout << "Preparing to install " << package_name << ":" << package_arch << " v" << package_version << std::endl; 
		
		// Проверка архитектуры пакета
		if(check_architecture(package_arch)) {
			printf("[\e[31mE\e[39m] Error occured while installing %s: Package architecture mismatch!", package_name.c_str());
			exit(2);
		}
		
		fs::create_directories(VAR_PATH + "/packages/" + package_name + "_" + package_version);
		fs::copy("/tmp/pako/package/PAKO/files", VAR_PATH + "/packages/" + package_name + "_" + package_version + "/files");
		fs::copy("/tmp/pako/package/PAKO/info", VAR_PATH + "/packages/" + package_name +  "_" + package_version + "/info");
		
		std::ifstream files_list(VAR_PATH + "/packages/" + package_name +  "_" + package_version + "/files");
		std::string filepath;
	
		while(getline(files_list, filepath)) {
		
			if(fs::is_directory("/tmp/pako/package/source/" + filepath)) {
				fs::create_directory(PREFIX + filepath);
				/* DEBUG
				* printf("directory: %s\n", filepath.c_str());
				*/
			} else {
				std::ifstream source_file("/tmp/pako/package/source/" + filepath, std::ios::binary);
				std::ofstream destination_file(PREFIX + filepath, std::ios::binary);
				destination_file << source_file.rdbuf();
				/* DEBUG
				* printf("file: %s\n", filepath.c_str());
				*/
				source_file.close();
				destination_file.close();
			}
		}
		files_list.close();	
		infofile.close();
	}
}
