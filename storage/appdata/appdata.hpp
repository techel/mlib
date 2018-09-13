#pragma once

#include <string>
#include <filesystem>
#include <fstream>

namespace mlib::storage::appdata
{

//
// exceptions
//

struct RetrieveError : public std::runtime_error
{
public: RetrieveError(const std::string &name) : runtime_error(".storage.appdata.retrieve (" + name + ")") {}
};

//
// file location type
//

enum class Type
{
    Storage, Resource
};

//
// Stores/Loads application data in %appdata%\app or $HOME/app and loads application data from app's resource directory.
// On MacOS, this is the bundle's resource folder, elsewhere it is the app's working directory.
// Dots (.) in paths, 'appsubdir' denote subdirectories.
//

class Appdata
{
public:
    Appdata(const std::string &appsubdir, const std::string &resourcesubdir);

    std::fstream open(const std::string &path, int flags, Type type);

private:
    std::filesystem::path StoragePath, ResourcePath;

};

}
