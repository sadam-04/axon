#include "../nayuki-qr/qrcodegen.hpp"

std::string getFilename(std::string path);
std::string parseRequestPath(char* cbuf);
std::string getFileExt(std::string fname);

std::string randAN(unsigned int len);

std::string getExecutableDirectory();
std::string getParentDirectory(const std::string& fullPath);

std::string loadFile(std::string filename);
std::string GetLANIPAddress();

std::string makebmp(qrcodegen::QrCode qr);