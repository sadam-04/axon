struct AXONSETTINGSCONF
{
    //command line settings
    std::string executable;
    std::string served_file;
    bool url_scrambling;

    //settings.txt
    std::string host;
    unsigned int port;
    unsigned int sendbgcolor;
    unsigned int recvbgcolor;
    unsigned int textcolor;
    unsigned int qr_dark_color;
    unsigned int qr_light_color;
    std::string save_to;
};

AXONSETTINGSCONF loadSettings(unsigned int argc, char** argv, std::string fname);