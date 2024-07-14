#include "servemd/core/server.hpp"
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>

using servermd::core::Server;

Server::Server()
{
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == -1) throw std::runtime_error("Failed to open TCP socket");

    if (bind(socketfd, (sockaddr*)&addr, sizeof(addr)) != 0)
        throw std::runtime_error(std::strerror(errno));
}

Server::~Server() { }

void Server::run()
{
    std::cout << "Server started listening on port: " << ntohs(addr.sin_port) << std::endl;

    if (listen(socketfd, 10) != 0) throw std::runtime_error("Failed to listen");

    auto clientfd = accept(socketfd, 0, 0);
    if (clientfd == -1) throw std::runtime_error("Failed to accept connection");

    auto readBytes = recv(clientfd, &buffer, BUFFER_SIZE, 0);
    if (readBytes == -1) throw std::runtime_error("Failed to read request to buffer");
    std::string req(&buffer[0], readBytes);

    std::cout << " --- REQUEST ---\n" << req << std::endl;

    std::string
        res("HTTP/1.0 200 OK\n"
            "Content-Type: text/html\n"
            "Content-Length: 22\n"
            "\n\n"
            "<h1> HELLO WORLD </H1>");
    send(clientfd, res.data(), res.size(), 0);
}
