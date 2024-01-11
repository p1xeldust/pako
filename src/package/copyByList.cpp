/**
 * Written by Paul Goldstein, December 2023.
 */

#include <filesystem>
#include <fstream>
#include <string>

#include "package.h"

#include "../essential/dialog.h"
#include "../essential/o.h"
extern Output output;
extern Dialog dialog;

using std::string, std::getline, std::ifstream, std::filesystem::create_directories, std::filesystem::is_regular_file, std::filesystem::is_directory, std::filesystem::copy_file;

void copyByList(string tmpSourcePath, ifstream& listFile, Package& package)
{
    for(string line; getline(listFile, line);) {
        if(is_regular_file((string)PREFIX + line)) {
            // Do solve dialog
            output.warn("File already exists, triggering dialog");
            switch (dialog.solve_copy((string)PREFIX + line, package))
            {
            case 1:
                copy_file(tmpSourcePath + "/" + line, (string)PREFIX + "/" + line, std::filesystem::copy_options::update_existing);
                break;
            case -1:
                output.msg("See ya!");
                exit(EXIT_SUCCESS);
                break;
            default:
                printf("lol");
                break;
            };
        }
        else if(is_directory(line))
            create_directories((string)PREFIX + "/" + line);
        else {
            copy_file(tmpSourcePath + "/" + line, (string)PREFIX + "/" + line, std::filesystem::copy_options::skip_existing);
        }
    }
    return;
}
