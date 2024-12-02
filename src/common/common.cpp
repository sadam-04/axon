#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <random>
#include <filesystem>
#include <queue>
#include <SFML/Graphics.hpp>
#include "../common/common.hpp"
#include "../settings/settings.hpp"
#include "../nayuki-qr/qrcodegen.hpp"

extern AXONSETTINGSCONF SETTINGS;
extern std::string ROOT_DIR;

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <iphlpapi.h>
    #pragma comment(lib, "ws2_32.lib")
    #pragma comment(lib, "iphlpapi.lib")
#endif

#ifdef __APPLE__
    #include <mach-o/dyld.h>
    #include <ifaddrs.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
#endif

std::string getExecutableDirectory() {
    #ifdef _WIN32
        char buffer[MAX_PATH];
        GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    #endif
    #ifdef __APPLE__
        char buffer[PATH_MAX];
        _NSGetExecutablePath(buffer, &size);
    #endif
    std::string fullPath(buffer);
    return fullPath.substr(0, fullPath.find_last_of("\\/"));
}

#ifdef _WIN32
std::string GetLANIPAddress() {
    ULONG bufferSize = 0;
    GetAdaptersAddresses(AF_INET, 0, nullptr, nullptr, &bufferSize); // Determine buffer size
    IP_ADAPTER_ADDRESSES* adapters = (IP_ADAPTER_ADDRESSES*)malloc(bufferSize);

    if (GetAdaptersAddresses(AF_INET, 0, nullptr, adapters, &bufferSize) == NO_ERROR) {
        for (IP_ADAPTER_ADDRESSES* adapter = adapters; adapter; adapter = adapter->Next) {
            // Ignore adapters that are not currently up
            if (adapter->OperStatus != IfOperStatusUp) continue;

            // Ignore loopback and tunnel adapters
            if (adapter->IfType == IF_TYPE_SOFTWARE_LOOPBACK || adapter->IfType == IF_TYPE_TUNNEL) continue;

            // Ignore virtual adapters based on description
            std::wstring description = adapter->Description;
            if (description.find(L"Virtual") != std::string::npos ||
                description.find(L"VMware") != std::string::npos ||
                description.find(L"Hyper-V") != std::string::npos ||
                description.find(L"TAP") != std::string::npos) {
                continue;
            }

            // Loop through unicast addresses
            for (IP_ADAPTER_UNICAST_ADDRESS* addr = adapter->FirstUnicastAddress; addr; addr = addr->Next) {
                sockaddr_in* sa_in = (sockaddr_in*)addr->Address.lpSockaddr;

                // Convert to a string
                char ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(sa_in->sin_addr), ip, INET_ADDRSTRLEN);

                // Exclude loopback and link-local addresses
                if (strncmp(ip, "127.", 4) != 0 && strncmp(ip, "169.254", 7) != 0) {
                    free(adapters);
                    return std::string(ip);
                }
            }
        }
    }

    free(adapters);
    return "";
}
#endif

#ifdef __APPLE__
std::string GetLANIPAddress() {
    struct ifaddrs *ifaddr, *ifa;
    std::string ipAddress;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return "";
    }

    //for each interface...
    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr)
            continue;

        //                              (ip4)
        if (ifa->ifa_addr->sa_family == AF_INET) {
            if (ifa->ifa_flags & IFF_LOOPBACK)
                continue;

            char addr[INET_ADDRSTRLEN];
            //get ip address
            void *addr_ptr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;

            if (inet_ntop(AF_INET, addr_ptr, addr, sizeof(addr)) != nullptr) {
                std::string interfaceName(ifa->ifa_name);
                if (interfaceName.find("vbox") != std::string::npos || 
                    interfaceName.find("vmnet") != std::string::npos) {
                    continue;
                }

                ipAddress = addr;
                break;
            }
        }
    }

    freeifaddrs(ifaddr);

    return ipAddress;
}
#endif

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

std::string makebmp(qrcodegen::QrCode qr, unsigned int light_color, unsigned int dark_color)
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
    bmp += pack32(light_color);
    bmp += pack32(dark_color);
    
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

//Applies a postfix to a filename, before the file extension. For example, file.txt + aBc12 = fileaBc12.txt
std::string applyFilenamePostfix(std::string filename, std::string postfix)
{
    char ext_mark = '.';

    size_t ext = filename.find_last_of(ext_mark);
    if (ext == std::string::npos)
        return filename + postfix;

    return filename.substr(0, ext) + postfix + filename.substr(ext, filename.size()-1);
}

int pop_filerc(std::queue<FileRC> &q)
{
    FileRC filerc = q.front();
    q.pop();

    std::string filepath = ROOT_DIR + '/' + SETTINGS.save_to;
    std::string filename = filerc.filename;

    std::string fullname = filepath + filename;
    
    while (fileExists(fullname))
    {
        std::cout << "file " << fullname << " already exists\n";
        fullname = filepath + applyFilenamePostfix(filename, randAN(5));
    }

    std::filesystem::create_directory(ROOT_DIR + '/' + SETTINGS.save_to); //c++17
    std::ofstream file(fullname, std::ios::binary);
    if (file) {
        file.write(filerc.data.c_str(), filerc.data.size());
        file.close();
        return 0;
    } else {
        std::cerr << "Failed to save file " << filerc.filename << "(" << fullname << ")\n";
        return 1;
    }
}

void trimWhitespaceL(std::string &str)
{
    while (!str.empty() && (std::string(" \n\r\t").find(str[0]) != std::string::npos))
        str.erase(0, 1);
}

void trimWhitespaceR(std::string &str)
{
    while (!str.empty() && (std::string(" \n\r\t").find(str.back()) != std::string::npos))
        str.pop_back();
}

void trimWhitespace(std::string &str)
{
    trimWhitespaceL(str);
    trimWhitespaceR(str);
}

bool fileExists(std::string filename)
{
    std::ifstream check_exists(filename);
    if (check_exists.is_open())
    {
        std::string word;
        check_exists >> word;
        std::cout << "WORD:" << word << '\n';
        std::cout << "FILE EXISTS!!!\n";
        return true;
    }
    std::cout << "FILE DOES NOT EXIST!!!\n";
    return false;
}