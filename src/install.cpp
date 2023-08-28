#include <archive.h>
#include <archive_entry.h>
#include <cstdint>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <unistd.h>
#include <stdlib.h>

// Убрать или добавить отладку getPackageInfo. Она часто сбивает с толку, но порой необходима.
#ifdef DEBUG
	#define DEBUG_GPI 0
#endif

// Обычные x86/x86_64
#ifdef __x86_64
	#define HOSTARCH "amd64"
#endif
#ifdef __x86
	#define HOSTARCH "i386"
#endif

// Любимые PowerPC
#ifdef __powerpc__
	#define HOSTARCH "ppc"
#endif
#ifdef __powerpc64__
	#define HOSTARCH "ppc64"
#endif

// Мипс и ель
#ifdef __mips__
	#define HOSTARCH "mips"
#endif
#ifdef __mipsel__
	#define HOSTARCH "mipsel"
#endif

// И ручки тут как тут
#ifdef __arm__
	#define HOSTARCH "arm"
#endif
#ifdef __aarch64__
	#define HOSTARCH "arm64"
#endif


using namespace std;
namespace fs = std::filesystem;

bool mkTempDir(string path) {

	#ifdef DEBUG
		cout << "[Debug] Trying to create temp directory" << endl;
	#endif

	try {
		if(fs::exists(path))
			fs::remove_all(path);
		fs::create_directories(path);
	}

	catch(const std::exception& e) {
		#ifdef DEBUG
			cout << "[Debug:mkTempDir] " << e.what() << endl;
		#endif
		return false;
	}

	return true;
}

bool unpack(string filePath, string destinationPath) {
struct archive* a;
struct archive_entry* entry;

a = archive_read_new();
archive_read_support_filter_xz(a);
archive_read_support_format_tar(a);

	if (archive_read_open_filename(a, filePath.c_str(), 10240) != ARCHIVE_OK) {
		archive_read_free(a);
		cerr << "[Error] Unable to extract " << filePath << endl;
		return false;
	}

	while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
	const char* entryPath = archive_entry_pathname(entry);
		string extractPath = destinationPath + "/" + entryPath;

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

		while (archive_read_data_block(a, &buff, &size, &offset) == ARCHIVE_OK)
			fwrite(buff, 1, size, file);

		fclose(file);

		mode_t mode = archive_entry_perm(entry);
		chmod(extractPath.c_str(), mode);
	}

	archive_read_close(a);
	archive_read_free(a);
	return true;
}

bool getPackageInfo(string dataFilePath, string packageInfo[3]) {
	ifstream dataFile(dataFilePath, ios::in);
	if(!dataFile.good())
		return false;
	for(string line; getline(dataFile, line); dataFile.good()) {
		if (line.find(':') != std::string::npos) {
			string type = line.substr(0, line.find(':'));
			string value = line.substr(line.find(':') + 1);
			type.erase(type.find_last_not_of(" \t") + 1);
			value.erase(0, value.find_first_not_of(" \t"));

			if(type == "name") {
				packageInfo[0] = value;
			}
			else if(type == "version") {
				packageInfo[2] = value;
			}
			else if(type == "arch") {
				packageInfo[1] = value;
			}
		}
	}
	for(int i = 0; i < 3; i++) {
		#if DEBUG_GPI == 1
			cout << "[Debug:PackageInfo " << i << " ]" << packageInfo[i] << endl;
		#endif

		if(packageInfo[i].size() <= 0)
			return false;
	}
	dataFile.close();
	return true;
}
// Далась мне эта функция с трудом, забыл приколы со строками наглухо.

bool checkDeps(string dataFilePath) {
	vector<string> dependencies;
	ifstream dataFile(dataFilePath, ios::in);
	string packageInfo[3];
	for(string line; getline(dataFile, line); dataFile.good()) {

		if(line.find(':') != std::string::npos) {
			string type = line.substr(0, line.find(':'));
			string value = line.substr(line.find(':') + 1);

			type.erase(type.find_last_not_of(" \t") + 1);
			value.erase(0, value.find_first_not_of(" \t"));

			if(type == "deps" && value.size() > 0) {

				#ifdef DEBUG
	                cout << "[Debug:checkDeps] Dependency found: '" << value << "'" << endl;
                #endif
				dependencies.push_back(std::move(value));
			} else
				continue;
		}
	}
	if(dependencies.size() <= 0)
		return true;
	else
		for(size_t i=0; i<dependencies.size(); ++i) {
		
			for(const auto& entry : fs::directory_iterator((string)TMP_PATH)) {
        	    getPackageInfo(entry.path().string() + "/package/PAKO/info", packageInfo);
		
			    if(dependencies[i] == packageInfo[0]) {
			
			    	#ifdef DEBUG
                     	cout << "[Debug:checkDeps] Dependency solved: '" << dependencies[i] << "'" << endl;
                    #endif
                    dependencies.erase(std::remove(dependencies.begin(), dependencies.end(), packageInfo[0]), dependencies.end());
					break;
				}
	        }
			
			for(const auto& entry : fs::directory_iterator((string)VAR_PATH + "/packages/")) {
				getPackageInfo(entry.path().string() + "/info", packageInfo);
		
				if(dependencies[i] == packageInfo[0]) {
		
					#ifdef DEBUG
						cout << "[Debug:checkDeps] Dependency solved: '" << dependencies[i] << "'" << endl;
					#endif
					break;
					dependencies.erase(std::remove(dependencies.begin(), dependencies.end(), packageInfo[0]), dependencies.end());
				}
			}
		}
	if(dependencies.size() > 0) {
		cout << "[i] Unable to detect dependent packages:";
		
		for(size_t i = 0; i < dependencies.size(); i++)
				cout << " '" << dependencies[i] << "'";
		cout << endl;
		return false;
	}
	return true;
}

bool checkArch(string packageArch) {

	#ifdef DEBUG
		cout << "[Debug:Arch] Host: " << HOSTARCH << endl;
		cout << "[Debug:Arch] Package: " << packageArch << endl;
	#endif

	if((string)HOSTARCH != packageArch)
		return false;

	return true;
}

uint8_t Install(int argc, char* argv[]) {
	if(geteuid() != 0) {
		cerr << "[i] Action requires superuser privileges." << endl;
		return 127;
	}

	// Параллельно создаём временные папки для распаковки пакетов
	for(int i = 2; i<argc; i++) {
		
		if(!mkTempDir((string)TMP_PATH + "/package" + to_string(i-2))) {
			cerr << "[Error] Unable to create temporary directory." << endl;
			return 2;
		}

		cout << "[i] Extracting " << argv[i] << endl;

		// Параллельно распаковываем пакеты в временные папки
		if(!unpack(fs::current_path().string() + "/" + argv[i], (string)TMP_PATH + "/package" + to_string(i-2))) {
			cout << "fail " << argv[i] << endl;
			return 3;
		}
	}

	// Получаем информацию о пакете и добавляем контрольные файлы в систему пакетов 
	for(int i = 2; i<argc; i++) {
		string tmpDataFilePath = (string)TMP_PATH + "/package" + to_string(i-2) + "/package/PAKO/info";
		vector<string> deps;
		string packageInfo[3];

		if(!getPackageInfo(tmpDataFilePath, packageInfo)) {
			cerr << "[Error] Error while processing " << argv[i] << endl;
			return 4;
		}
		else
			cout << "[i] Processing " << argv[i] << endl;

		if(!checkDeps(tmpDataFilePath)) {
			cout << "[Error] Unable to solve dependencies" << endl;
			return 5;
		}

		if(!checkArch(packageInfo[1])) {
			string ans;
			cout << "[i] Uncompatible architecture detected. Proceed? [y/N] ";
			cin >> ans;
			if(ans != "y" || ans != "yes")
				return 6;
		}

		try {
			fs::create_directories("/var/lib/pako/packages/" + packageInfo[0]);
			fs::copy((string)TMP_PATH + "/package" + to_string(i-2) + "/package/PAKO", (string)VAR_PATH +"/packages/" + packageInfo[0], fs::copy_options::overwrite_existing | fs::copy_options::recursive);
		}

		catch(const exception& e) {
			cerr << "[Error] Unable to extract package control files." << endl;
			#ifdef DEBUG
			cout << "[Debug:CopyControlFiles]" << e.what() << endl;
			#endif
			return 7;
		}
		if(fs::exists((string)VAR_PATH +"/packages/" + packageInfo[0] + "/install"))
			system(("sh " + (string)VAR_PATH +"/packages/" + packageInfo[0] + "/install" + " --preinst").c_str());		
	}

	// Копируем содержимое всех распакованных пакетов в префикс
	for(int i = 2; i<argc; i++) {
		string packageInfo[3];
		vector<string> deps;

		if(!getPackageInfo((string)TMP_PATH + "/package" + to_string(i-2) + "/package/PAKO/info", packageInfo)) {
			cerr << "[Error] Error while processing " << argv[i] << endl;
			return 4;
		}

		cout << "[i] Installing " << argv[i] << endl;

		try {
			fs::create_directories((string)VAR_PATH + "/packages/" + packageInfo[0]);
			fs::copy((string)TMP_PATH + "/package" + to_string(i-2) + "/package/source", (string)PREFIX, fs::copy_options::update_existing | fs::copy_options::recursive);
		}

		catch(const exception& e) {
			cerr << "[Error] Unable to extract package contents." << endl;
			#ifdef DEBUG
			cout << "[Debug:CopySourceFiles]" << e.what() << endl;
			#endif
			return 7;
		}
		if(fs::exists((string)VAR_PATH +"/packages/" + packageInfo[0] + "/install"))
			system(("sh " + (string)VAR_PATH +"/packages/" + packageInfo[0] + "/install" + " --postinst").c_str());
	}
	#if EASTER_EGG == 1
		std::cout << "Message from developer: Thank you!" << std::endl;
	#endif
	return 0;
}
