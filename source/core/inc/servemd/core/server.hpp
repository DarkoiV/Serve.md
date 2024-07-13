#pragma once

namespace servermd::core {
struct Server
{
    Server();
    ~Server();

    Server(Server&)            = delete;
    Server(Server&&)           = delete;
    Server operator=(Server&)  = delete;
    Server operator=(Server&&) = delete;
};
} // namespace servermd::core
