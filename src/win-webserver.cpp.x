#include <winsock2.h>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <vector>
#include "common/common.hpp"

#pragma comment(lib, "Ws2_32.lib")

void handleClient(SOCKET clientSocket, std::string correct_path, std::string &http_200_response, std::string &http_404_response) { //suggested content disposition
    // Buffer for receiving data
    char buffer[1024] = {0};
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Receive failed with error: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        return;
    }
    buffer[bytesReceived] = '\0';

    //get the requested url path
    std::string request_path = parseRequestPath(buffer);
    
    const char *http_response = http_404_response.c_str();
    unsigned int http_response_size = http_404_response.size();

    //if requested path matches the correct one, send 200 response, otherwise send 404.
    if (request_path == std::string(correct_path))
    {
        http_response = http_200_response.c_str();
        http_response_size = http_200_response.size();
    }

    // Send the response to the client
    send(clientSocket, http_response, http_response_size, 0);

    // Close the client socket
    closesocket(clientSocket);
}

int webserver(std::string filepath, std::string &data, unsigned int port, std::string url_path) {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
        return 1;
    }

    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Setup the server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    //build http_200_response
    std::string ext, content_disposition = "attachment", mimetype = "application/octet-stream";
    ext = getFileExt(getFilename(filepath));

    std::vector<std::pair<std::vector<std::string>, std::string>> known_types = {
        {{"jpg", "jpeg"}, "image/jpeg"},
        {{"png"}, "image/png"},
        {{"pdf"}, "application/pdf"},
        {{"mp4"}, "video/mp4"},
        {{"txt"}, "text/plain"}
    };

    for (int t = 0; t < known_types.size(); t++)
    {
        for (int e = 0; e < known_types[t].first.size(); e++)
        {
            if (ext == known_types[t].first[e]) mimetype = known_types[t].second;
        }
    }

    std::string http_200_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: " + mimetype + "\r\n"
        "Content-Length: " + std::to_string(data.size()) + "\r\n"
        //+ "Content-Disposition: " + content_disposition + (content_disposition=="attachment" ? ("; filename=\""+getFilename(filepath)+"\"") : ("")) + "\r\n"
        + "Content-Disposition: " + content_disposition + (content_disposition=="attachment" ? ("; filename=\""+getFilename(filepath)+"\"") : ("")) + "\r\n"
        + "Connection: close\r\n\r\n"
        + data;

    // Simple HTTP response
    std::string body_404 =
        "<!DOCTYPE html><html><head><title>Fore, oh fore</title></head>"
        "<body><h1>404 Not Found</h1>\n<p>check your url path or scan the QR code</p></body></html>";
    std::string http_404_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: " + std::to_string(body_404.size()) + "\r\n"
        + "Content-Disposition: inline\r\n"
        + "Connection: close\r\n\r\n"
        + body_404;

    // Main server loop
    std::vector<std::thread> responses;
    while (true) {
        // Accept a client socket
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
            continue;
        }

        // Handle the client request
        responses.push_back(std::thread(handleClient, clientSocket, url_path, std::ref(http_200_response), std::ref(http_404_response)));
    }

    // Cleanup
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
