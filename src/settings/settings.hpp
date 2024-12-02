struct AXONSETTINGSCONF
{
    std::string host;
    unsigned int port;
    unsigned int sendbgcolor;
    unsigned int recvbgcolor;
    unsigned int textcolor;
    unsigned int qr_dark_color;
    unsigned int qr_light_color;
    bool url_scrambling;
    std::string save_to;
};

AXONSETTINGSCONF loadSettings(unsigned int argc, char** argv, std::string fname);