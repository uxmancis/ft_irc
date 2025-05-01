#include <iostream>
#include <cstdlib>
#include "Server/Server.hpp"
#include "Colors.hpp"

int main(int argc, char** argv)
{
    if (argc != 3) 
    {
        std::cerr << RED "Uso correcto: ./ircserv [PUERTO] [CONTRASEÑA]\n" RESET;
        return (1);
    }
    int port = std::atoi(argv[1]);
    if (port <= 0 || port > 65535) 
    {
        std::cerr << RED "El puerto debe estar entre 1 y 65535.\n" RESET;
        return (1);
    }
    try
    {
        Server server(port, argv[2]);
        server.run();
        return (0);
    }
    catch (const std::exception& e)
    {
        std::cerr << RED "💥 Error crítico: " RESET << e.what() << std::endl;
        return (1);
    }
    return (0);
}

