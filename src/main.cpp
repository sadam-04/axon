#include <iostream>
#include <string>
#include <thread>
#include <fstream>
#include <sstream>
#include "nayuki-qr/qrcodegen.hpp"
#include "webserver.hpp"
#include "dataloader.hpp"

char QR_1_STR[2] = {(char)219, (char)219};
char QR_0_STR[2] = {(char)32, (char)32};

#define QR_FALSE

typedef std::pair<std::string, unsigned int> HOST;

HOST getHost(std::string filename)
{
    HOST host;
    
    std::ifstream hostFile(filename);
    if (!hostFile.is_open())
    {
        std::cerr << "Unable to open " << filename << "\n";
        exit(1);
    }

    hostFile >> host.first;
    std::string portbuf;
    hostFile >> portbuf;
    host.second = std::stoi(portbuf);

    return host;
}

int main(int argc, char* argv[]) {

    std::string fname;

    if (argc == 2)
    {
        fname = argv[1];
    }
    else
    {
        std::cout << "target filename:";    
        std::cin >> fname;

        if (fname.empty())
        {
            std::cerr << "Invalid filename\n";
            exit(1);
        }
    }

    std::string data = loadFile(fname);

    HOST host = getHost("host.txt");

    std::string url = "http://" + host.first + ":" + std::to_string(host.second) + "/file";

    // Create a QR Code object
    qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(url.c_str(), qrcodegen::QrCode::Ecc::LOW);

    // Print the QR Code to the console
    int border = 4;
    for (int y = -border; y < qr.getSize() + border; y++) {
        for (int x = -border; x < qr.getSize() + border; x++) {
            std::cout << (qr.getModule(x, y) ? (const char *)QR_1_STR : (const char *)QR_0_STR);
        }
        std::cout << std::endl;
    }

    webserver(fname, data, host.second);

    return 0;
}