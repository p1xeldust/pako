/**
 * Written by Paul Goldstein, December 2023.
 */

#include <filesystem>
#include <fstream>
#include <string>

#include "../package/package.h"
#include "config.h"
#include "dialog.h"
#include "output.h"

void copyByList(const std::filesystem::path& tmpSourcePath, std::ifstream &listFile, Package &package)
{

    for (std::string line; std::getline(listFile, line);)
    {
        if (std::filesystem::is_regular_file(configParams.prefixPath / line))
        {
            if (dialog.solveCopy(package, configParams.prefixPath / line))
                std::filesystem::copy_file(tmpSourcePath / line, configParams.prefixPath / line, std::filesystem::copy_options::update_existing);
        }
        else if (std::filesystem::is_directory(tmpSourcePath / line))
            std::filesystem::create_directories(configParams.prefixPath / line);
        else
            std::filesystem::copy_file(tmpSourcePath / line, configParams.prefixPath / line, std::filesystem::copy_options::skip_existing);
    }
}
