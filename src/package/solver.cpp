#include <fstream>
#include <filesystem>

bool Package::solveDeps(string packageName) {
    vector<string> deps;
    for(const auto entry : fs::directory_iterator((string)VAR_PATH + "/packages")) {
        if(entry.path().string() == (string)VAR_PATH + "/packages/" + packageName + ".info")
            continue;
        ifstream dataFile(entry.path().string() + ".info", ios::in);

        for(string line; getline(dataFile, line); dataFile.good()) {

            if(line.find("deps:") != std::string::npos) {
                istringstream depList(line.substr(line.find(':') + 1).erase(0, line.find_first_not_of(" \t")));
                for(std::string dep; depList >> dep;)
                    if(dep == packageName)
                        dependencies.push_back(std::move(packageName));
                }
          }
|    }
    if(dependencies.size() <= 0) {
        out.debugmsg("SolveDeps: solved dependencies!");
        return true;
    } else {
        out.msg("Unable to solve dependencies. These packages:");
        for(size_t i=0; i < deps.size(); i++)
            out.msg("Package '" + dependencies[i] + "' depends on " + packageName);
        out.msg("Use --force-remove to avoid this check.");
        return false;
    }
}

void Package::removePackageSource(string listFilePath) {
    std::vector<std::string> files;
    std::ifstream listFile(listFilePath);
    for(std::string line; getline(listFile, line); listFile.good()) {
        files.push_back((string)PREFIX + "/" + line);
        //out.debugmsg(" Added " + (string)PREFIX + "/" + line + " to remove list");
    }
    /* Удаление файлов */
    for(size_t i=0; i<files.size(); i++) {
        if(std::filesystem::exists(files[i]) && !fs::is_symlink(files[i])) {
            fs::remove(files[i]);
            out.debugmsg("Removed file " + files[i]);
        }
    }
    for(size_t i=0; i<files.size(); i++) {
        if(std::filesystem::exists(files[i]) && std::filesystem::is_directory(files[i])) {
            std::filesystem::remove(files[i]);
            out.debugmsg("Removed directory " + files[i]);
        }
    }
    for(size_t i=0; i<files.size(); i++) {
        if(std::filesystem::exists(files[i]) && std::filesystem::is_symlink(files[i])) {
            std::filesystem::remove(files[i]);
            out.debugmsg("Removed symlink " + files[i]);
        }
    }
}
