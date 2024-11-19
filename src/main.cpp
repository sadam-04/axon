#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <fstream>
#include <sstream>
#include "nayuki-qr/qrcodegen.hpp"
#include "webserver.hpp"
#include "dataloader.hpp"
#include "qr_to_bmp.hpp"
#include "window.hpp"
#include "randompath.hpp"
#include "settings/settings.hpp"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

char QR_1_STR[2] = {(char)219, (char)219};
char QR_0_STR[2] = {(char)32, (char)32};

#define QR_FALSE

std::string getParentDirectory(const std::string& fullPath) {
    size_t lastSlashPos = fullPath.find_last_of("\\/");
    if (lastSlashPos != std::string::npos) {
        return fullPath.substr(0, lastSlashPos + 1); // Include trailing slash
    }
    return "";
}

std::string getExecutableDirectory() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    std::string fullPath(buffer);
    return fullPath.substr(0, fullPath.find_last_of("\\/"));
}

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
    std::string url_path = '/' + generatePath();
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