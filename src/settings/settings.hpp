struct AXONSETTINGSCONF
{
    std::string host;
    unsigned int port;
    unsigned int sendbgcolor;
    unsigned int recvbgcolor;
    std::string save_to;
};

AXONSETTINGSCONF loadSettings(std::string fname);