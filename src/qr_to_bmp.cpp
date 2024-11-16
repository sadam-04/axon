#include <iostream>
#include "nayuki-qr/qrcodegen.hpp"

#define BMP_DATA_START (0x0)

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