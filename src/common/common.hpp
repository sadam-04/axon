#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#include <queue>
#include "../nayuki-qr/qrcodegen.hpp"

std::string getFilename(std::string path);
std::string parseRequestPath(char* cbuf);
std::string getFileExt(std::string fname);

std::string randAN(unsigned int len);

std::string getExecutableDirectory();
std::string getParentDirectory(const std::string& fullPath);

std::string loadFile(std::string filename);
std::string GetLANIPAddress();

std::string makebmp(qrcodegen::QrCode qr, unsigned int color1, unsigned int color2);

void trimWhitespaceL(std::string &str);
void trimWhitespaceR(std::string &str);
void trimWhitespace(std::string &str);

bool fileExists(std::string filename);

struct FileRC {
    std::string filename;
    std::string data;
    std::string sender;
    bool approved;
};

int pop_filerc(std::queue<FileRC> &q);

#endif