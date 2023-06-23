#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "constants.hpp"

namespace fs = std::filesystem;

int list_f(int argc, char* argv[]) {
	std::string path = constants::VAR_PATH + "/packages/";
	std::cout << std::setw(15) << std::left << "PACKAGE"
					<< std::setw(8) << "VERSION"
					<< std::setw(6) << "ARCH"
					<< std::endl;
	if(argc > 2) {
		std::string package_name, package_version, package_arch;
		for(int i=2; i<argc; i++) {
			if(!fs::is_directory(constants::VAR_PATH + "/packages/" + argv[i]))
				return 1;
			std::ifstream info_file(constants::VAR_PATH + "/packages/" + argv[i] + "/info");
			
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
			std::cout << std::setw( 15 ) << std::left << package_name
							<< std::setw( 8 ) << package_version
							<< std::setw( 6 ) << package_arch
							<< std::endl;
		}
	}
    return 0;
}
