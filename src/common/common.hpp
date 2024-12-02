#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#include <queue>
#include <filesystem>
#include "../nayuki-qr/qrcodegen.hpp"

std::string getFilepath(std::string path);
std::string getFilename(std::string path);
std::string getFileExt(std::string fname);

std::string trimFileExt(std::string fname);

std::string parseRequestPath(char* cbuf);

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

class FileRC {
private:
    std::string path;

    std::string name;
    std::string postfix;
    std::string extension;

    std::string data;
    std::string sender;
    bool approved;
public:
    FileRC(std::string fullpath, std::string data, std::string sender);

    void print();

    std::string getShortName();
    std::string getFullName();

    std::string& getData();

    std::string getSender();

    bool is_approved();

    int save();
};

int pop_filerc(std::queue<FileRC> &q);

#endif