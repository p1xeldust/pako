#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include "vars.h"
using namespace std;

namespace fs = std::filesystem;

bool check_exist(std::string package_name) {
	std::ifstream package_folder(package_name + "/info");
	if(package_folder.good())
		return 0;
	return 1;
}
void remove_unique(const std::string& cur_package_name) {
	std::ifstream cur_pkgfile(cur_package_name + "/files");
	std::string cur_pkgline;

	while (std::getline(cur_pkgfile, cur_pkgline)) {
		bool mismatchFound = false;
		for (const auto& entry : std::filesystem::directory_iterator(constants::VAR_PATH+"/packages/")) {
			if (entry.is_directory()) {
				std::string cmp_package = entry.path().string();

				if (cmp_package != cmp_package) {
					std::ifstream oth_pkgfile(cmp_package + "/files");
					std::string oth_pkgline;

					while (std::getline(oth_pkgfile, oth_pkgline)) {
						if (cur_pkgline != oth_pkgline) {
							mismatchFound = true;
							std::cout << entry.path().string() << std::endl;
							break;
						}
					}

					if (mismatchFound)
						break;
				}
			}
		}

        if (mismatchFound) {
			if(fs::is_regular_file(constants::PREFIX + "/" + cur_pkgline))
				fs::remove(constants::PREFIX + "/" + cur_pkgline);
		}
	}
}
int remove_f(int argc, char* argv[]) {
	if(geteuid() != 0) {
		printf("[\e[31mE\e[39m] Operation requires superuser privilegies\n");
		return 127;
	}
	for(int i = 2; i<argc; i++) {
		std::string package_folder = constants::VAR_PATH + "/packages/" + argv[i];
		if(!fs::is_directory(package_folder)) {
			printf("[E] No such package");
			return 1;
		}
		ifstream files_list(package_folder + "/files");
		if(!files_list.good()) {
			printf("[E] No files list found, something gone wrong\n");
			return 2;
		}

		remove_unique(package_folder);
		fs::remove_all(package_folder);

	}
	return 0;
}


