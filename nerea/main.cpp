#include "Server/Server.hpp"

int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv [PORT] [PASSWORD]\n";
        return 1;
    }

    try
    {
        Server server(std::atoi(argv[1]), argv[2]);
        std::cout << "Iniciando servidor...\n";
        server.setup();
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "CHACHO ALGO MALO PASO: " << e.what(); 
        return 1;
    }
}
