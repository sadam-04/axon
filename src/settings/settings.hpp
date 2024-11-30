struct AXONSETTINGSCONF
{
    std::string host;
    unsigned int port;
    unsigned int sendbgcolor;
    unsigned int recvbgcolor;
    unsigned int textcolor;
    unsigned int qr_dark_color;
    unsigned int qr_light_color;
    std::string save_to;
};

AXONSETTINGSCONF loadSettings(std::string fname);