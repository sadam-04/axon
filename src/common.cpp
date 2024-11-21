#include <string>
#include <sstream>
#include <random>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

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

std::string getExecutableDirectory() {
    char buffer[1024];
    GetModuleFileNameA(nullptr, buffer, 1024);
    std::string fullPath(buffer);
    return fullPath.substr(0, fullPath.find_last_of("\\/"));
}

std::string getParentDirectory(const std::string& fullPath) {
    size_t lastSlashPos = fullPath.find_last_of("\\/");
    if (lastSlashPos != std::string::npos) {
        return fullPath.substr(0, lastSlashPos + 1); // Include trailing slash
    }
    return "";
}