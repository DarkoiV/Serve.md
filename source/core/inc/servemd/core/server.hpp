#pragma once
#include <netinet/in.h>

namespace servermd::core {
struct Server
{
    Server();
    ~Server();

    Server(Server&)            = delete;
    Server(Server&&)           = delete;
    Server operator=(Server&)  = delete;
    Server operator=(Server&&) = delete;

    void run();

  private:
    constexpr static int BUFFER_SIZE{1024};

    int         socketfd;
    sockaddr_in addr{AF_INET, htons(8080), 0};
    char        buffer[BUFFER_SIZE]{0};
};
} // namespace servermd::core
