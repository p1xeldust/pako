#ifndef PAKO_CONFIG
#define PAKO_CONFIG

#include <filesystem>
#include <fstream>

class ConfigParams {
private:
    std::string varPrefixPath;
    std::string varTmpPath;
    std::string varVarPath;
    void updateVariables();
public:
    std::filesystem::path prefixPath;
    std::filesystem::path tmpPath;
    std::filesystem::path varPath;
    std::filesystem::path dbPath = varPath / "packages";
    void ReadConfigFile(const std::string& filePath);
};

extern ConfigParams configParams;

#endif