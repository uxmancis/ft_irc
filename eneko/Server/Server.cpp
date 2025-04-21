#include "Server.hpp"

Server::Server(int port, const std::string& password) : _password(password) 
{
    _serverFD = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFD < 0)
        throw std::runtime_error("Error creating socket");

    int opt = 1;
    setsockopt(_serverFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(_serverFD, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
        throw std::runtime_error("Bind failed");

    if (listen(_serverFD, SOMAXCONN) < 0)
        throw std::runtime_error("Listen failed");

    fcntl(_serverFD, F_SETFL, O_NONBLOCK);
    PollManager pollManager(_serverFD);
    std::cout << "Servidor iniciado en el puerto " << port << std::endl;
    std::cout << "Presiona ENTER para salir.\n";
    std::cin.get();
    close(_serverFD);
}

Server::~Server() 
{
    if (_serverFD != -1)
        close(_serverFD);
}

