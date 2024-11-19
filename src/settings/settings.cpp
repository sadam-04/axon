#include <fstream>
#include <iostream>
#include <string>
#include "settings.hpp"
#include "../gethost.hpp"

std::string defaults = 
    "host=auto\n"
    "port=8082\n"
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

    std::string line;
    std::getline(sett_file, line);
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
        }
    } while (std::getline(sett_file, line));

    // Do value processing here
    if (settings.host == "auto")
        settings.host = GetLANIPAddress();

    return settings;
}