#include <winsock2.h>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")
#define PORT 8080

std::string shortenPath(std::string path)
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
    std::cout << "Request Path: " << path << '\n';

    return path;
}

void handleClient(SOCKET clientSocket, std::string filename, std::string data) {
    // Buffer for receiving data
    char buffer[1024] = {0};
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Receive failed with error: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        return;
    }

    buffer[bytesReceived] = '\0'; // Null-terminate the buffer

    // Simple HTTP response
    std::string response_body =
        "<!DOCTYPE html><html><head><title>For, oh fore</title></head>"
        "<body><h1>nothing here</h1></body></html>";

    std::string response_type = 
        "text/html";

    std::string dlname = 
        "";

    std::string reqPath = parseRequestPath(buffer);
    if (reqPath == std::string("/file" /*+ filename*/))
    {
        std::cout << "DOWNLOAD FILE!!!!!!!!!\n";
        response_type = "application/octet-stream";
        //response_body = filename;
        response_body = data;
        dlname = "Content-Disposition: attachment; filename=" + shortenPath(filename) + "\r\n";
    }
    else
    {
        std::cout << "reqPath: " + reqPath + '\n';
        std::cout << "filename: " + std::string("/" + filename) + '\n';
    }

    std::string resp2 = "more data nevergoon";

    std::cout << "\n\n" << filename << "\n\n";

    //application/octet-stream
    std::string httpResponse =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: " + response_type + "\r\n"
        "Content-Length: " + std::to_string(response_body.size() /*+ resp2.size()*/) + "\r\n"
        + dlname
        + "Connection: close\r\n\r\n"
        + response_body;

    std::cout << "\n\n\nSENDING RESPONSE:\n" << httpResponse << "\n\n\n\n";

    // Log the request
    std::cout << "Received request:\n" << buffer << std::endl;

    // Send the response to the client
    send(clientSocket, httpResponse.c_str(), httpResponse.size(), 0);
    //send(clientSocket, resp2.c_str(), resp2.size(), 0);

    // Close the client socket
    closesocket(clientSocket);
}

int webserver(std::string filename, std::string data) {
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
    serverAddr.sin_port = htons(PORT);

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

    std::cout << "Server is listening on port " << PORT << std::endl;

    std::vector<std::thread> responses;

    // Main server loop
    while (true) {
        // Accept a client socket
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
            continue;
        }

        // Handle the client request
        responses.push_back(std::thread(handleClient, clientSocket, filename, data));
    }

    // Cleanup
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
