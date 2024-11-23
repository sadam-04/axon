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
    std::string data;
    if (argc == 2)
        fname = argv[1];
        // Load target data from file
        data = loadFile(fname);

    // Build url
    std::string url_path_send = '/' + randAN(5);
    std::string url_path_recv = '/' + randAN(5);

    while (url_path_recv == url_path_send)
        url_path_recv = '/' + randAN(5);

    std::string url_send = "http://" + SETTINGS.host + ":" + std::to_string(SETTINGS.port) + url_path_send;
    std::string url_recv = "http://" + SETTINGS.host + ":" + std::to_string(SETTINGS.port) + url_path_recv;

    // Start webserver
    std::thread ws_thread(webserver, fname, std::ref(data), SETTINGS.port, url_path_send, url_path_recv);

    // Create QR
    qrcodegen::QrCode qr_send = qrcodegen::QrCode::encodeText(url_send.c_str(), qrcodegen::QrCode::Ecc::LOW);
    qrcodegen::QrCode qr_recv = qrcodegen::QrCode::encodeText(url_recv.c_str(), qrcodegen::QrCode::Ecc::LOW);
    std::string qr_bmp_send = makebmp(qr_send);
    std::string qr_bmp_recv = makebmp(qr_recv);

    // Create SFML window on main thread
    createWindow(qr_bmp_send, qr_bmp_recv, getFilename(fname), url_send, url_recv);

    return 0;
}