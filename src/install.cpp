#include "vars.h"
#include <string>
#include <filesystem>
#include <archive.h>
#include <archive_entry.h>
#include <fstream>
#include <iostream>

using namespace std;
namespace fs = std::filesystem;


int check_arch(std::string &cur_arch) {
	if(HOST_ARCH != cur_arch)
		return 1;
	return 2;
}

bool create_temp() {
	try {
		if(fs::is_directory(constants::TMP_PATH))
			fs::remove_all(constants::TMP_PATH);
		fs::create_directory(constants::TMP_PATH);
	}
	catch(const std::exception& e) {
		printf("[\e[31mE\e[39m] Unable to create temporary directory");
		#ifdef DEBUG
			printf("%s", e);
		#endif
		return false;
	}
	return true;
}

bool unpack(std::string filePath, std::string destinationPath) {
    struct archive* a;
    struct archive_entry* entry;
    int result;

    a = archive_read_new();
    archive_read_support_filter_xz(a);
    archive_read_support_format_tar(a);

    result = archive_read_open_filename(a, filePath.c_str(), 10240);
    if (result != ARCHIVE_OK) {
        archive_read_free(a);
        return true;
    }
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        const char* entryPath = archive_entry_pathname(entry);
        std::string extractPath = destinationPath + "/" + entryPath;

        if (archive_entry_filetype(entry) == AE_IFLNK) {
            const char* symlinkTarget = archive_entry_symlink(entry);
            symlink(symlinkTarget, extractPath.c_str());
            continue;
        }

        if (archive_entry_filetype(entry) == AE_IFDIR) {
            int mkdirResult = mkdir(extractPath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
            if (mkdirResult != 0) {
                archive_read_close(a);
                archive_read_free(a);
                return false;
            }
            continue;
        }

        FILE* file = fopen(extractPath.c_str(), "wb");
        if (!file) {
            archive_read_close(a);
            archive_read_free(a);
            return false;
        }
        const void* buff;
        size_t size;
        la_int64_t offset;
        while (archive_read_data_block(a, &buff, &size, &offset) == ARCHIVE_OK) {
            fwrite(buff, 1, size, file);
        }

        fclose(file);

        mode_t mode = archive_entry_perm(entry);
        chmod(extractPath.c_str(), mode);
    }

    archive_read_close(a);
    archive_read_free(a);
    return true;
}

int get_package_info(std::string path, std::string package_info[3]) {
	std::ifstream infofile(path); // Проверяем наличие info файла в пакете. Если есть - продолжаем
	if(!infofile.good())
		return 1;
	for(std::string line; getline(infofile, line, ' ');) {
		if (line == "name:") {
			getline(infofile, line);
			if(!(line.size() > 0)) {
				printf("[\e[31mE\e[39m] Unable to detect package name.\n");
				return 3;
			}
			package_info[0] = line;
			
		}
		if (line == "version:") {
			getline(infofile, line);
			if(!(line.size() > 0)) {
				printf("[\e[31mE\e[39m] Unable to detect package version.\n");
				return 3;
			}
			package_info[2] = line;
			
		}
		if (line == "arch:") {
			getline(infofile, line);
			if(!(line.size() > 0)) {
				printf("[\e[31mE\e[39m] Unable to detect package architecture.\n");
				return 3;
			}
			package_info[1] = line;
			
		}
	}
	return true;
}

int copy_package_information(std::string package_info[3]) {
	try {
		std::string VAR_PKG = constants::VAR_PATH + "/packages/" + package_info[0];
		if(fs::exists(constants::VAR_PATH + "/packages/" + package_info[0])) {
			return 2; // Already installed
		}
		fs::create_directories(VAR_PKG);
		fs::copy(constants::TMP_PATH + "/package.tmp", VAR_PKG + "/" + package_info[0] + ".pako");
		fs::remove(constants::TMP_PATH + "/package.tmp");
		fs::copy(constants::TMP_PATH + "/package/PAKO/files", VAR_PKG + "/files");
		fs::copy(constants::TMP_PATH + "/package/PAKO/info", VAR_PKG + "/info");

		if(fs::exists(constants::TMP_PATH + "/package/PAKO/install")) {
			fs::copy(constants::TMP_PATH + "/package/PAKO/install", constants::VAR_PATH + "/packages/" + package_info[0] + "/install");
		}
	} catch(const std::exception& e) {
		std::cout << e.what() << std::endl;
		return 0;
	}
	return 1;
}

int install_f(int argc, char* argv[]) { 
	if(geteuid() != 0) {
		printf("[\e[31mE\e[39m] Operation requires superuser privilegies\n");
		return 127;
	}
	for(int i =2; i<argc; i++) {
		//Расширяемый базис для пакета:
		std::string package_info[3];
		// Создаёт временную папку в TMP_PATH
		if(!create_temp())
			return 1;
			
		// Стадия распаковки
		fs::copy(std::filesystem::current_path().string() + "/" + argv[i], constants::TMP_PATH + "/package.tmp");
		printf("Unpacking %s...\n", argv[i]);
		if(!unpack(constants::TMP_PATH + "/package.tmp", constants::TMP_PATH)) {
			printf("[\e[31mE\e[39m] Unable to extract %s.\n", argv[i]);
			return 2;
		}
		
		//Стадия вычленения инфы пакета
		get_package_info(constants::TMP_PATH + "/package/PAKO/info", package_info);
		
		if(!check_arch(package_info[1]) == 1) {
			printf("[\e[31mE\e[39m] Package architecture mismatch.\n");
			return 4;
		}
		if(fs::exists(constants::VAR_PATH + "/packages/" + package_info[0] + "/install")) {
		cout << "Preparing " << package_info[0] << ":" << package_info[1] << " v" << package_info[2] << endl;
		system(("sh " + constants::VAR_PATH + "/packages/" + package_info[0] + "/install --preinstall").c_str());
		}
		cout << "Installing " << package_info[0] << ":" << package_info[1] << " v" << package_info[2] << endl;
		try {
			for (const auto& entry : fs::directory_iterator(constants::TMP_PATH + "/package/source/")) {
				const std::string entryPath = entry.path().string();
				const std::string entryName = entry.path().filename().string();
				const std::string destinationEntryPath = constants::PREFIX + "/" + entryName;
				fs::copy(entryPath, destinationEntryPath, fs::copy_options::recursive | fs::copy_options::copy_symlinks);
			}
		} catch(const std::exception& e) {}
		if(fs::exists(constants::VAR_PATH + "/packages/" + package_info[0] + "/install")) {
		cout << "Configuring " << package_info[0] << ":" << package_info[1] << " v" << package_info[2] << endl;
		system(("sh " + constants::VAR_PATH + "/packages/" + package_info[0] + "/install --postinstall").c_str());
		}
		cout << "Installed " << package_info[0] << ":" << package_info[1] << " v" << package_info[2] << endl;
	}	
	return 0; // отбивка успешного завершения
}
