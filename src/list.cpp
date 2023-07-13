#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "vars.h"

namespace fs = std::filesystem;

int list_f(int argc, char* argv[]) {
	std::string path = constants::VAR_PATH + "/packages/";
	if(!fs::exists(constants::VAR_PATH + "/packages/")) {
		if(geteuid() == 0)
			fs::create_directories(constants::VAR_PATH + "/packages/");
		else {
			printf("[\e[31mE\e[39m] Unable to open/create packages folder.\n");
			return 1;
		}
	}

	std::cout << std::setw(15) << std::left << "PACKAGE"
					<< std::setw(8) << "VERSION"
					<< std::setw(6) << "ARCH"
					<< std::endl;
	if(argc > 2) {
		std::string package_info[3];
		for(int i=2; i<argc; i++) {
			if(!fs::exists(constants::VAR_PATH + "/packages/" + argv[i]) && !fs::is_directory(constants::VAR_PATH + "/packages/" + argv[i])) {
			std::cout << std::setw(15) << std::left << argv[i]
							<< std::setw(8) << "unknown"
							<< std::setw(6) << "unknown"
							<< std::endl;
			}

			std::ifstream info_file(constants::VAR_PATH + "/packages/" + argv[i] + "/info");
			for(std::string line; getline(info_file, line, ' '); info_file.good()) {
				if(line == "name:") {
					getline(info_file, line);
					package_info[0] = line;
				}
				if(line == "version:") {
					getline(info_file, line);
					package_info[1] = line;
				}
				if(line == "arch:") {
					getline(info_file, line);
					package_info[2] = line;
				}
			}
			std::cout << std::setw(15) << std::left << package_info[0]
							<< std::setw(8) << package_info[1]
							<< std::setw(6) << package_info[2]
							<< std::endl;
		}

	} else {
		for (const auto & entry : fs::directory_iterator(path)) {
			std::string package_path = entry.path();
			std::ifstream info_file(package_path + "/info");
			std::string package_name, package_version, package_arch;
			for(std::string line; getline(info_file, line, ' '); info_file.good()) {
				if(line == "name:") {
					getline(info_file, line);
					package_name = line;
				}
				if(line == "version:") {
					getline(info_file, line);
					package_version = line;
				}
				if(line == "arch:") {
					getline(info_file, line);
					package_arch = line;
				}
			}
			std::cout << std::setw(15) << std::left << package_name
							<< std::setw(8) << package_version
							<< std::setw(6) << package_arch
							<< std::endl;
		}
	}
    return 0;
}
