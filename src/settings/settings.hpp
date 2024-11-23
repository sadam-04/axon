struct AXONSETTINGSCONF
{
    std::string host;
    unsigned int port;
    unsigned int bgcolor;
};

AXONSETTINGSCONF loadSettings(std::string fname);