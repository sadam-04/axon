#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

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