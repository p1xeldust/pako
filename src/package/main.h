#ifndef INSTALL_H
#define INSTALL_H

#include <vector>
#include <string>

int Install(std::vector<std::string> packagesAsFiles);
int Remove(std::vector<std::string> packages);
int List(std::vector<std::string> packages);
void Version();
void Help();
#endif
