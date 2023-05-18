#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include "unpack.cpp"
#include <filesystem>

#define PREFIX "/opt/"

using namespace std;

void install(int argc, char* argv[]) {
	for(int i=2; i<argc; i++) {
		
		if(!mkdir("/tmp/pako", 0777)) {
			printf("Error occured while creating temporary folder\n");
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
			printf("Error occured while unpacking %s: Bad package\n", argv[i]);
			exit(1);
		}
		package_tmp.close();
		remove("/tmp/pako/package.tmp"); // Удаление пакета из tmp
		
		std::ifstream infofile("/tmp/pako/package/PAKO/info"); // Проверяем наличие info файла в пакете. Если есть - продолжаем
		if(!infofile.good()) {
			printf("Error occured while unpacking %s: Is not a package\n", argv[i]);
			exit(1);
		}
		// ВСТАВИТЬ ПРОВЕРКИ ИМЕНИ, ВЕРСИИ, АРХИТЕКТУРЫ
		
		// Копирование содержимое пакета в PREFIX (PREFIX = "/")
		std::string filepath;
		std::ifstream files_list("/tmp/pako/package/PAKO/files");
		while(getline(files_list, filepath)) {
			std::string folders = filepath;
			int filename_length = folders.find_last_of("/");
			if (filename_length >= 0)
				folders = folders.substr(0, filename_length); // Удаляем из пути файла имя файла для последующего создания папок и копирования файлов
			std::filesystem::create_directories(PREFIX + folders);
			if(std::filesystem::is_directory("/tmp/pako/package/source/"+filepath)) {
				std::filesystem::create_directories(PREFIX + filepath);
				printf("directory: %s\n", filepath.c_str());
			} else {
				std::ifstream source_file("/tmp/pako/package/source/"+filepath);
				std::ofstream destination_file(PREFIX + filepath);
				destination_file << source_file.rdbuf();
				printf("file: %s\n", filepath.c_str());
				source_file.close();
				destination_file.close();
			}
		std::filesystem::remove_all("/tmp/pako");
		}
	}
}

