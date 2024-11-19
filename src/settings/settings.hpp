struct AXONSETTINGSCONF
{
    std::string host;
    unsigned int port;
};

AXONSETTINGSCONF loadSettings(std::string fname);