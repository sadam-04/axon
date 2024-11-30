#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "settings.hpp"
#include "../common/common.hpp"

std::string defaults = 
    "host=auto\n"
    "port=8080\n"
    "\n"
    "# colors are encoded in RGBA format #\n"
    "sendbgcolor=FF353CFF\n"
    "recvbgcolor=3C35FFFF\n"
    "textcolor=FFFFFFFF\n"
    "qr_dark_color=000000FF\n"
    "qr_light_color=FFFFFFFF\n"
    "\n"
    "save_to=saved/\n"
;

void createDefaultSettings(std::string fname)
{
    std::ofstream sett_file(fname);

    if (!sett_file.is_open())
    {
        std::cerr << "Unable to open file " + fname + ". An attempt to create this file also failed\n";
        exit(0);
    }

    sett_file.write(defaults.c_str(), defaults.size());
    sett_file.close();
}

void parseSettingsFile(std::istream &f, AXONSETTINGSCONF &settings)
{
    std::string line;
    std::getline(f, line);
    do
    {
        std::string::size_type eq = line.find('=');
        if (eq != std::string::npos)
        {
            std::string field = line.substr(0, eq);
            std::string val = line.substr(eq+1, line.size()-1);
            while (val.back() == '\n' || val.back() == '\r' || val.back() == ' ')
                val.pop_back();
            if (field == "port")
                settings.port = std::stoi(val);
            if (field == "host")
                settings.host = val;
            if (field == "sendbgcolor")
                settings.sendbgcolor = std::stoul(val, nullptr, 16);
            if (field == "recvbgcolor")
                settings.recvbgcolor = std::stoul(val, nullptr, 16);
            if (field == "textcolor")
                settings.textcolor = std::stoul(val, nullptr, 16);
            if (field == "qr_dark_color")
                settings.qr_dark_color = std::stoul(val, nullptr, 16);
            if (field == "qr_light_color")
                settings.qr_light_color = std::stoul(val, nullptr, 16);
            if (field == "save_to")
                settings.save_to = val;
        }
    } while (std::getline(f, line));

    // Do value processing here
    if (settings.host == "auto")
        settings.host = GetLANIPAddress();
}

AXONSETTINGSCONF loadSettings(std::string fname)
{
    AXONSETTINGSCONF settings;

    std::ifstream sett_file(fname);
    if (!sett_file.is_open())
    {
        createDefaultSettings(fname);
        sett_file.open(fname);
        if (!sett_file.is_open())
        {
            std::cerr << "Unable to open file " + fname;
            exit(0);
        }
    }

    std::stringstream def_set_ss(defaults);
    parseSettingsFile(def_set_ss, settings);
    parseSettingsFile(sett_file, settings);

    return settings;
}