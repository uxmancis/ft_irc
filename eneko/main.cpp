#include "Server/Server.hpp"
#include <iostream>

int main(int argc, char** argv)
{
    if (argc != 3)
       return (std::cerr << "Usage: ./ircserv [PORT] [PASSWORD]\n", 1);
    Server server(atoi(argv[1]), argv[2]);
    return (0);
}