#pragma once

#include <string>
#include <fstream>
#include <filesystem>

#include <mlib/log/loginterface.hpp>

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
// Slahes (/) in paths, 'appsubdir' and 'resourcesubdir' denote subdirectories. '..' are ignored.
//

class Appdata
{
public:
    Appdata(log::ILog &l, const std::string &appsubdir, const std::string &resourcesubdir);

    std::filesystem::path filepath(const std::string &path, Type type);
    std::fstream open(const std::string &path, std::ios::openmode flags, Type type);

private:
    log::ILog *Log;
    std::filesystem::path StoragePath, ResourcePath;
};

}
