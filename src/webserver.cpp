#include <winsock2.h>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

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

    return path;
}

std::string getFileExt(std::string fname)
{
    while (fname.find('.') != std::string::npos)
        fname.erase(0, 1);

    return fname;
}

void handleClient(SOCKET clientSocket, std::string filename, std::string data, std::string url_path, std::string mimetype, std::string sug_cont_dispo) { //suggested content disposition
    // Buffer for receiving data
    char buffer[1024] = {0};
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Receive failed with error: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        return;
    }

    buffer[bytesReceived] = '\0';

    // Simple HTTP response
    std::string response_body =
        "<!DOCTYPE html><html><head><title>For, oh fore</title></head>"
        "<body><h1>nothing here</h1></body></html>";

    std::string cont_dispo = "inline";

    std::string reqPath = parseRequestPath(buffer);
    
    
    if (reqPath == std::string(url_path))
    {
        //response_type = "application/octet-stream";
        response_body = data;
        cont_dispo = sug_cont_dispo;
    }
        

    std::string httpResponse =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: " + mimetype + "\r\n"
        "Content-Length: " + std::to_string(response_body.size()) + "\r\n"
        + "Content-Disposition: " + cont_dispo + (cont_dispo=="attachment" ? ("; filename="+shortenPath(filename)+"\r\n") : (""))
        + "Connection: close\r\n\r\n"
        + response_body;

    // Send the response to the client
    send(clientSocket, httpResponse.c_str(), httpResponse.size(), 0);

    // Close the client socket
    closesocket(clientSocket);
}

int webserver(std::string filename, std::string data, unsigned int port, std::string url_path) {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    std::string ext, content_disposition = "attachment", mimetype = "application/octet-stream";
    ext = getFileExt(filename);

    if (ext == "jpg" || ext == "jpeg")
        mimetype = "image/jpeg";
    else if (ext == "png")
        mimetype = "image/png";
    else if (ext == "pdf")
        mimetype = "application/pdf";
    else if (ext == "mp4")
    {
        mimetype = "video/mp4";
        content_disposition = "inline";
    }
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
        responses.push_back(std::thread(handleClient, clientSocket, filename, data, url_path, mimetype, content_disposition));
    }

    // Cleanup
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
