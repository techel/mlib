#include "settinghost.hpp"

#include <cassert>

namespace mlib::storage::settings
{

SettingHost::SettingHost(log::ILog &l, appdata::Appdata &a, std::string filename) : Log(&l), Appdata(&a), Filename(std::move(filename))
{
}

ClientTicket SettingHost::registerClient(IClient &client, std::string name)
{
    assert(Clients.find(name) == Clients.end());

    Clients[std::move(name)] = &client;

    auto it = std::prev(Clients.end());
    ClientTicket t([this, it]()
    {
        Clients.erase(it);
    });

    if(auto sect = Archive.find(name); sect)
        client.loadSettings(*sect);

    return t;
}


void SettingHost::loadSettings()
{
    auto file = Appdata->open(Filename, std::ios::in, appdata::Type::Storage);
    if(file)
    {
        Archive.deserialize(file, [&](unsigned int line, archive::ini::Malformation err)
        {
            Log->log(log::Error, ".storage.settings", "INI file line " + std::to_string(line) + ": " + archive::ini::malformationString(err));
        });

        for(auto &[name, client] : Clients)
        {
            if(auto *sect = Archive.find(name))
                client->loadSettings(*sect);
            else
                client->defaultSettings();
        }
    }
    else
        loadDefaults();
}

void SettingHost::loadDefaults()
{
    for(auto c : Clients)
        c.second->defaultSettings();
}

void SettingHost::storeSettings()
{
    for(const auto &[name, client] : Clients)
    {
        auto &sect = Archive.section(name);
        client->storeSettings(sect);
    }

    auto file = Appdata->open(Filename, std::ios::out, appdata::Type::Storage);
    if(file)
        Archive.serialize(file);
}


}