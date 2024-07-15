#include "servemd/core/server.hpp"
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

using servermd::core::Server;

Server::Server()
{
    tcpfd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcpfd == -1) throw std::runtime_error("Failed to open TCP socket");

    if (bind(tcpfd, (sockaddr*)&addr, sizeof(addr)) != 0)
        throw std::runtime_error(std::strerror(errno));

    epollfd = epoll_create1(0);
    if (epollfd == -1) throw std::runtime_error("Failed to create epoll fd");

    epoll_event newTcpConnection;
    newTcpConnection.events  = EPOLLIN;
    newTcpConnection.data.fd = tcpfd;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, tcpfd, &newTcpConnection) == -1)
        throw std::runtime_error("Failed to add new tcp connection event to epoll");
}

Server::~Server()
{
    if (epollfd != -1) close(epollfd);
    if (tcpfd != -1) close(tcpfd);
}

void Server::run()
{
    if (listen(tcpfd, 100) != 0)
        throw std::runtime_error(std::string("Cannot start to listen on port: " +
                                             std::to_string(ntohs(addr.sin_port))));

    std::cout << "Server started listening on port: " << ntohs(addr.sin_port) << std::endl;

    epoll_event events[MAX_EPOLL_EVENTS];
    while (true)
    {
        int evNo = epoll_wait(epollfd, events, MAX_EPOLL_EVENTS, 30000);
        if (evNo == -1) throw std::runtime_error("Epoll error");

        for (int i = 0; i < evNo; i++)
        {
            if (events[i].data.fd == tcpfd) acceptNewConnection();
            else if (events[i].events & EPOLLRDHUP) closeTcpPipe(events[i].data.fd);
            else respondToRequest(events[i].data.fd);
        }
    }
}

void Server::acceptNewConnection()
{
    int clientfd = accept(tcpfd, 0, 0);
    if (clientfd == -1) throw std::runtime_error("Failed to accept connection");

    epoll_event newHttpRequest;
    newHttpRequest.events  = EPOLLIN | EPOLLRDHUP;
    newHttpRequest.data.fd = clientfd;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &newHttpRequest) == -1)
        throw std::runtime_error("Failed to add newHttpRequest event to epoll");
}

void Server::closeTcpPipe(int clientfd)
{
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, clientfd, nullptr) == -1)
        throw std::runtime_error("Failed to delete newHttpRequest event from epoll");
}

void Server::respondToRequest(int clientfd)
{
    int readBytes = recv(clientfd, &buffer, BUFFER_SIZE, 0);
    if (readBytes == -1) throw std::runtime_error("Failed to read request to buffer");

    std::string req(&buffer[0], readBytes);

    std::cout << " --- REQUEST [" << clientfd << "] ---\n" << req << std::endl;

    std::string
        res("HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 21\r\n"
            "\r\n"
            "<h1> HELLO WORLD <h1>");
    send(clientfd, res.data(), res.size(), 0);
}
