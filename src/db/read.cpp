
#include <fstream>
#include <sstream>
#include <string>

#include "../package/package.h"
#include "../common/config.h"
#include "database.h"

Package Database::GetPackage(std::string packageName)
{
    Package package;
    std::ifstream dbFile(configParams.dbPath);

    for (std::string line; std::getline(dbFile, line);)
    {
        std::stringstream ss(line);
        std::string type, value;
        ss >> type;
        ss >> value;
        if (type == "name" && value == packageName)
        {
            package.name = value;
            break;
        }
        else if (dbFile.eof())
        {
            return package;
        }
    }
    for (std::string line; std::getline(dbFile, line);)
    {
        if (line == "EOPE")
            break;
        std::stringstream ss(line);
        std::string type, value;
        ss >> type;
        std::getline(ss >> std::ws, value);

        if (type == "version" && !value.empty())
            package.version = value;
        else if (type == "arch" && !value.empty())
            package.arch.name = value;
        else if (type == "type" && !value.empty())
            package.meta = static_cast<packageMetaType>(stoi(value));
        else if (type == "dependencies" && !value.empty())
        {
            std::stringstream dependencies(value);
            for (std::string dependency; dependencies >> dependency;)
                package.dependencies.push_back(dependency);
        }
        else if (type == "conflicts" && !value.empty())
        {
            std::stringstream conflicts(value);
            for (std::string conflict; conflicts >> conflict;)
                package.conflicts.push_back(conflict);
        }
        else if (type == "description" && !value.empty())
            package.description = value;
        else if (type == "listfile" && !value.empty())
            package.files.listFilePath = value;
        else if (type == "scriptfile" && !value.empty())
            package.files.scriptFilePath = value;
        else
            continue;
    }
    dbFile.close();
    return package;
}

bool Database::IsIn(std::string packageName)
{
    std::ifstream dbFile(configParams.dbPath);
    for (std::string line; std::getline(dbFile, line);)
        if (line == "name " + packageName)
            return true;
    return false;
}