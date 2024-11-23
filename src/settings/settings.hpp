struct AXONSETTINGSCONF
{
    std::string host;
    unsigned int port;
    unsigned int bgcolor;
    std::string save_to;
};

AXONSETTINGSCONF loadSettings(std::string fname);