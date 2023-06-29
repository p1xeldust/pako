#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

#include "constants.h"

using namespace std;

void check_remove(const std::string& cur_package_name) {
	std::ifstream cur_pkgfile(cur_package_name + "/files");
	std::string cur_pkgline;

	while (std::getline(cur_pkgfile, cur_pkgline)) {
		bool mismatchFound = false;

		for (const auto& entry : std::filesystem::directory_iterator(constants::VAR_PATH+"/packages/")) {
			if (entry.is_directory()) {
				std::string oth_pkgname = entry.path().string();

				if (oth_pkgname != oth_pkgname) {
					std::ifstream oth_pkgfile(oth_pkgname + "/files");
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

int remove(int argc, char* argv[]) {

	//	Проверка UID на root
	if(geteuid() != 0) {
		printf("[\e[31mE\e[39m] Operation requires superuser privilegies\n");
		return 127;
	}
	
	for(int i = 2; i<argc; i++) {
		std::vector<std::string> folders;
		std::string folder = constants::VAR_PATH + "/packages/" + argv[i];
		if(!fs::is_directory(constants::VAR_PATH + "/packages/" + argv[i])) {
			printf("[E] No such package");
			return 1;
		}
		ifstream files_list(constants::VAR_PATH + "/packages/" + argv[i] + "/files");
		if(!files_list.good()) {
			printf("[E] No files list found, something gone wrong\n");
			return 2;
		}

		check_remove(constants::VAR_PATH + "/packages/" + argv[i]);
		fs::remove_all(constants::VAR_PATH + "/packages/" + argv[i]);

	}
	return 0;
}

