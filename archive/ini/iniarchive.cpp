#include "iniarchive.hpp"

#include <ostream>
#include <istream>
#include <algorithm>
#include <cassert>
#include <cctype>
#include <regex>

namespace mlib::archive::ini
{

//
// errors
//

const char *malformationString(Malformation e)
{
    const char *s[] = { "NoIdentifier", "DuplicateSection", "DuplicateEntry", "BadLine" };
    return s[static_cast<int>(e)];
}

//
// INI file section
//

void INISection::set(std::string name, std::string value)
{
    Entries[std::move(name)] = std::move(value);
}

std::string *INISection::find(const std::string &ent)
{
    auto it = Entries.find(ent);
    return it == Entries.end() ? nullptr : &it->second;
}

const std::string *INISection::find(const std::string &ent) const
{
    auto it = Entries.find(ent);
    return it == Entries.end() ? nullptr : &it->second;
}

//
// INI file
//

void INIArchive::serialize(std::ostream &out) const
{
    for(const auto &[name, sect] : Sections)
    {
        out << "[" + name + "]\n";

        for(const auto &[ename, entry] : sect.entries())
            out << ename << " = " << entry << '\n';
    }
}

static void trimTrailing(std::string &s)
{
    s = std::regex_replace(s, std::regex("\\s+$"), "");
}

void INIArchive::deserialize(std::istream &src, std::function<void(unsigned int, Malformation)> errorhandler)
{
    std::string currsectname;
    INISection currsect;

    unsigned int lineno = 0;
    std::string line;
    while(std::getline(src, line))
    {
        lineno++;

        static const std::regex blank("\\s*([;#].*)?");
        static const std::regex head("\\s*\\[\\s*([^\\]]+)\\]\\s*[;#]?.*");
        static const std::regex entry("\\s*([^=]+)=\\s*([^;#]*)[;#]?.*");

        std::smatch match;

        if(std::regex_match(line, match, blank))
            continue;

        else if(std::regex_match(line, match, head))
        {
            if(!currsectname.empty())
                Sections[std::move(currsectname)] = std::move(currsect);

            currsectname = match[1].str();

            trimTrailing(currsectname);
            if(line.empty())
                errorhandler(lineno, Malformation::NoIdentifier);

            if(Sections.find(currsectname) != Sections.end())
                errorhandler(lineno, Malformation::DuplicateSection);
        }

        else if(std::regex_match(line, match, entry))
        {
            auto entname = match[1].str();
            auto entval = match[2].str();

            trimTrailing(entname);
            trimTrailing(entval);

            if(entname.empty())
                errorhandler(lineno, Malformation::NoIdentifier);

            if(currsect.find(entname))
                errorhandler(lineno, Malformation::DuplicateEntry);

            currsect.set(std::move(entname), std::move(entval));
        }
        else
            errorhandler(lineno, Malformation::BadLine);
    }

    if(!currsectname.empty())
        Sections[std::move(currsectname)] = std::move(currsect);
}

void INIArchive::deserialize(std::istream &src)
{
    deserialize(src, [&](unsigned int lineno, Malformation m) { throw FileMalformed(lineno, m); });
}

INISection &INIArchive::section(const std::string &name)
{
    return Sections[name];
}
INISection *INIArchive::find(const std::string &sect)
{
    auto it = Sections.find(sect);
    return it != Sections.end() ? &it->second : nullptr;
}
const INISection *INIArchive::find(const std::string &sect) const
{
    auto it = Sections.find(sect);
    return it != Sections.end() ? &it->second : nullptr;
}

}