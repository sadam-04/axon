#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <random>
#include "../nayuki-qr/qrcodegen.hpp"

std::string getFilename(std::string path)
{
    while (path.find('/') != std::string::npos)
        path.erase(0, 1);

    while (path.find('\\') != std::string::npos)
        path.erase(0, 1);

    return path;
}

std::string parseRequestPath(char* cbuf)
{
    std::string buf = cbuf;

    std::stringstream stream(buf);

    std::string token;
    for (std::string token; token != "GET"; stream >> token) {};

    std::string path;
    stream >> path;

    return path;
}

std::string getFileExt(std::string fname)
{
    while (fname.find('.') != std::string::npos)
        fname.erase(0, 1);

    return fname;
}

std::string randAN(unsigned int len) {
    // Define the characters to choose from (letters and digits)
    const std::string charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::string randomString;

    // Create a random device and engine
    std::random_device rd;
    std::mt19937 gen(rd());  // Mersenne Twister random number generator
    std::uniform_int_distribution<> dis(0, charset.size() - 1);  // Uniform distribution

    // Generate a random string of the specified length
    for (int i = 0; i < len; ++i) {
        randomString += charset[dis(gen)];  // Append a random character
    }

    return randomString;
}

std::string getParentDirectory(const std::string& fullPath) {
    size_t lastSlashPos = fullPath.find_last_of("\\/");
    if (lastSlashPos != std::string::npos) {
        return fullPath.substr(0, lastSlashPos + 1); // Include trailing slash
    }
    return "";
}

std::string loadFile(std::string filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Unable to open file " + filename + "\n";
        exit(1);
    }

    std::ostringstream sstr;
    sstr << file.rdbuf();

    return sstr.str();
}

std::string pack32(int32_t value) {
    std::string binaryString(4, '\0'); // Initialize a 4-byte string filled with null characters

    // Pack the integer into the string in big-endian order
    binaryString[3] = static_cast<char>((value >> 24) & 0xFF);
    binaryString[2] = static_cast<char>((value >> 16) & 0xFF);
    binaryString[1] = static_cast<char>((value >> 8) & 0xFF);
    binaryString[0] = static_cast<char>(value & 0xFF);

    return binaryString;
}

std::string pack16(int16_t value) {
    std::string binaryString(2, '\0'); // Initialize a 4-byte string filled with null characters

    // Pack the integer into the string in big-endian order
    binaryString[1] = static_cast<char>((value >> 8) & 0xFF);
    binaryString[0] = static_cast<char>(value & 0xFF);

    return binaryString;
}

unsigned int makebyte(bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7) {
    unsigned int byte = 0; // Initialize the byte to 0

    byte += (b0 ? 1 : 0);
    byte = byte << 1;
    byte += (b1 ? 1 : 0);
    byte = byte << 1;
    byte += (b2 ? 1 : 0);
    byte = byte << 1;
    byte += (b3 ? 1 : 0);
    byte = byte << 1;
    byte += (b4 ? 1 : 0);
    byte = byte << 1;
    byte += (b5 ? 1 : 0);
    byte = byte << 1;
    byte += (b6 ? 1 : 0);
    byte = byte << 1;
    byte += (b7 ? 1 : 0);

    return byte;
}

std::string makebmp(qrcodegen::QrCode qr)
{
    std::string bmp;
    
    int qrsize = qr.getSize();

    // Header
    bmp += 'B';
    bmp += 'M';
    bmp += pack32(62 + (qrsize * qrsize / 2));
    bmp += pack32(0);
    bmp += pack32(62);

    // Info Header
    bmp += pack32(40);
    bmp += pack32(qrsize);
    bmp += pack32(qrsize);
    bmp += pack16(1);
    bmp += pack16(1);
    bmp += pack32(0); //compression
    bmp += pack32(0); //image size
    bmp += pack32(3780); //XpixelsPerM
    bmp += pack32(3780); //YpixelsPerM
    bmp += pack32(2); //colors used
    bmp += pack32(2); //important colors

    //Color Table
    bmp += pack32(0xFF000000);
    bmp += pack32(0xFFFFFFFF);

    
    bool bits[8] = {0};
    for (int y = 0; y < qrsize; y++) {
        int byteidx = 0;
        std::string scanline = "";
        for (int x = 0; x < qrsize; x++) {
            if (byteidx == 8)
            {
                byteidx = 0;
                
                unsigned int v = makebyte(bits[0], bits[1], bits[2], bits[3], bits[4], bits[5], bits[6], bits[7]);

                for (int i = 0; i < 8; i++)
                    bits[i] = false;

                scanline += v;
            }
            bits[byteidx++] = qr.getModule(x, qrsize - y - 1);
        }

        unsigned int v = makebyte(bits[0], bits[1], bits[2], bits[3], bits[4], bits[5], bits[6], bits[7]);

        for (int i = 0; i < 8; i++)
            bits[i] = false;

        scanline += v;

        bmp += scanline;
    }

    return bmp;
}