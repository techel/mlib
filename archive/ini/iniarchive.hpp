#pragma once

#include <iosfwd>
#include <string>
#include <map>
#include <stdexcept>
#include <functional>

namespace mlib::archive::ini
{

//
// errors
//

enum Malformation
{
    NoIdentifier, DuplicateSection, DuplicateEntry, BadLine
};

const char *malformationString(Malformation e);

struct FileMalformed : public std::runtime_error
{
public: FileMalformed(unsigned int line, Malformation e) : runtime_error(".archive.ini.badfile." + std::string(malformationString(e)) + " (" + std::to_string(line) + ")") {}
};

//
// INI file section
//

class INISection
{
public:
    const auto &entries() const { return Entries; }

    void set(std::string name, std::string value);
    std::string *find(const std::string &ent);
    const std::string *find(const std::string &ent) const;

private:
    std::map<std::string, std::string> Entries;
};

//
// INI file
//
// - supports global entries (section with empty name)
// - leading/trailing whitespaces in section names, entry names and values are trimmed
// - comments introduced with '#' or ';'
// - empty lines and trailing comments allowed
// - duplicate sections/entries and quoted values not supported
//

class INIArchive
{
public:
    void serialize(std::ostream &out) const;
    void deserialize(std::istream &src, std::function<void(unsigned int, Malformation)> errorhandler);
    void deserialize(std::istream &src); //throws exception on error

    INISection &section(const std::string &name);
    INISection *find(const std::string &sect);
    const INISection *find(const std::string &sect) const;

private:
    std::map<std::string, INISection> Sections;
};

}