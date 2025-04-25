#include "Server/Server.hpp"
#include <iostream>

int main(int argc, char** argv)
{
    if (argc != 3)
       return (std::cerr << RED "Usage: ./ircserv [PORT] [PASSWORD]\n" RESET, 1);
    try
    {
        Server server(std::atoi(argv[1]), argv[2]);
        return (0);
    }
    catch (const std::exception& e)
    {
        std::cerr << RED "CHACHO ALGO MALO PASO: " RESET << e.what(); 
        return (1);
    }
}
