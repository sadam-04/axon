#include <iostream>
#include <string>
#include <thread>
#include <fstream>
#include <sstream>
#include "nayuki-qr/qrcodegen.hpp"
#include "webserver.hpp"
#include "window.hpp"
#include "common/common.hpp"
#include "settings/settings.hpp"

char QR_1_STR[2] = {(char)219, (char)219};
char QR_0_STR[2] = {(char)32, (char)32};

std::string ROOT_DIR;
AXONSETTINGSCONF SETTINGS;

int main(int argc, char* argv[]) {

    ROOT_DIR = getExecutableDirectory(); //DOES NOT include trailing slash/
    SETTINGS = loadSettings(ROOT_DIR + "\\server.properties");

    std::string fname;

    if (argc == 2)
        fname = argv[1];
    else
    {
        std::cout << "Enter target filepath:";
        std::cin >> fname;

        if (fname.empty())
        {
            std::cerr << "Invalid filename\n";
            exit(1);
        }
    }

    // Load target data from file
    std::string data = loadFile(fname);

    // Build url
    std::string url_path = '/' + randAN(5);
    std::string url = "http://" + SETTINGS.host + ":" + std::to_string(SETTINGS.port) + url_path;
    
    // Start webserver
    std::thread ws_thread(webserver, fname, std::ref(data), SETTINGS.port, url_path);

    // Create QR
    qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(url.c_str(), qrcodegen::QrCode::Ecc::LOW);
    std::string qr_bmp = makebmp(qr);

    // Create SFML window on main thread
    createWindow(qr_bmp, fname, url);

    return 0;
}