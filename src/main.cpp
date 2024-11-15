#include <iostream>
#include <string>
#include <thread>
#include "qrcodegen.hpp"
#include "webserver.hpp"
#include "dataloader.hpp"

char QR_1_STR[2] = {(char)219, (char)219};
char QR_0_STR[2] = {(char)32, (char)32};

#define QR_FALSE

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

    //fname = "file";



    ///////////
    //fname = "file.txt";
    ///////////

    //webserver(fname, loadFile(fname));

    std::string host = "192.168.1.204:8080";



    std::string url = "http://" + host + "/file";

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

    std::thread webserver_thread(webserver, fname, data);

    webserver_thread.join();

    return 0;
}