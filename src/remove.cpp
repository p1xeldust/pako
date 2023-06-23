#include <filesystem>
#include <string>
#include <fstream>

#include "constants.hpp"

using namespace std;

int remove(int argc, char* argv[]) {
	if(geteuid() != 0) {
		printf("[\e[31mE\e[39m] Operation requires superuser privilegies\n");
		return 127;
	}
	for(int i = 2; i<argc; i++) {
		if(!fs::is_directory(constants::VAR_PATH + "/packages/" + argv[i])) {
			printf("[E] No such package");
			return 1;
		}
		ifstream files_list(constants::VAR_PATH + "/packages/" + argv[i] + "/files");
		if(!files_list.good()) {
			printf("[E] No files list found, something gone wrong\n");
			return 2;
		}
		std::string filepath;
		while(getline(files_list, filepath)) {
		
			if(fs::is_directory(constants::PREFIX + filepath)) {
				printf("directory: %s\n", filepath.c_str());
			} else {
				int nmatch = 0;
				for (const auto & entry : fs::directory_iterator(constants::VAR_PATH + "/packages/")) {
					std::string other_packages = entry.path();
					if(other_packages == constants::VAR_PATH + "/packages/")
						break;
					std::ifstream File1(constants::VAR_PATH + "/packages/" + argv[i] + "/files");
					std::ifstream File2("list2");
					for(std::string lineA; std::getline(File1, lineA); File1.good()) {
						for(std::string lineB; std::getline(File2, lineB); File2.good()) {
							if(File1.tellg() == File2.tellg())
								break;
							if (lineA == lineB) {
								nmatch = nmatch + 1;
							} else {
								nmatch = 0;
							}
						}
					}
				}
				if(nmatch == 0) {
					fs::remove(constants::PREFIX + filepath);
				}
			}
		}
		std::filesystem::remove_all(constants::VAR_PATH + "/packages/" + argv[i]);
	}
	
	return 0;
}

