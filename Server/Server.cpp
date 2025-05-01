#include "Server.hpp"
#include "../Poll/PollManager.hpp"
#include <iostream>
#include <stdexcept>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

Server::Server(int port, const std::string& password) : _serverFD(-1), _password(password), _port(port) 
{
    _setupSocket();
}

Server::~Server() 
{
    if (_serverFD != -1)
        close(_serverFD);
}

Server::Server(const Server& other) : _serverFD(-1), _password(other._password), _port(other._port) 
{
    _setupSocket();
}

Server& Server::operator=(const Server& other) 
{
    if (this != &other) 
    {
        _password = other._password;
        _port = other._port;
        if (_serverFD != -1)
            close(_serverFD);
        _setupSocket();
    }
    return (*this);
}

void Server::_setupSocket() 
{
    _serverFD = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFD < 0)
        throw std::runtime_error("Error creando el socket");

    int opt = 1;
    setsockopt(_serverFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(_serverFD, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
        throw std::runtime_error("Error en bind");

    if (listen(_serverFD, SOMAXCONN) < 0)
        throw std::runtime_error("Error en listen");

    fcntl(_serverFD, F_SETFL, O_NONBLOCK);
}

void Server::run() 
{
    std::cout << "Servidor iniciado en el puerto " << _port << std::endl;
    PollManager pollManager(_serverFD, _password);
    pollManager.run();
}
