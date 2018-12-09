#pragma once

#include <map>
#include <string>
#include <functional>

#include <mlib/util/ticket.hpp>
#include <mlib/log/loginterface.hpp>
#include <mlib/archive/ini/iniarchive.hpp>
#include <mlib/storage/appdata/appdata.hpp>
#include <mlib/interface/keyvalueinterface.hpp>

namespace mlib::storage::settings
{

//
// setting client
//

using IKeyvalue = interfaces::IKeyvalue<std::string, std::string>;

struct IClient
{
    virtual ~IClient() = default;

    virtual void storeSettings(IKeyvalue &arch) const = 0;
    virtual void loadSettings(const IKeyvalue &arch) = 0;
    virtual void defaultSettings() = 0;
};

//
// setting host
//

using ClientTicket = util::Ticket;

class SettingHost
{
public:
    SettingHost(log::ILog &l, appdata::Appdata &a, std::string filename);

    ClientTicket registerClient(IClient &client, std::string name);

    void storeSettings();
    void loadSettings();
    void loadDefaults();

    SettingHost(SettingHost&&) = delete;
    SettingHost(const SettingHost&) = delete;
    SettingHost &operator=(SettingHost&&) = delete;
    SettingHost &operator=(const SettingHost&&) = delete;

private:
    log::ILog *Log;
    appdata::Appdata *Appdata;
    std::string Filename;
    std::map<std::string, IClient*> Clients;
    archive::ini::INIArchive Archive;
};

}