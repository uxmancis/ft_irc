#include "Server/Server.hpp"
#include <iostream>

int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv [PORT] [PASSWORD]\n";
        return 1;
    }

    int port = atoi(argv[1]);
    std::string password = argv[2];

    Server server(port, password);
    if (!server.setup()) {
        std::cerr << "Error iniciando el servidor.\n";
        return 1;
    }
    return 0;
}