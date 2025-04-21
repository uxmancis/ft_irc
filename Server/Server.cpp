#include "Server.hpp"

Server::Server(int port, const std::string& password) : _port(port), _serverFd(-1), _password(password) {}

Server::~Server() 
{
    if (_serverFd != -1)
        close(_serverFd);
}

bool Server::setup(void) 
{
    _serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFd < 0) 
        return (std::cerr << "Error socket\n", false);


    int opt = 1;
    setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = inet_addr("10.14.8.3");

    if (bind(_serverFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
        return (std::cerr << "Error bind\n", false);

    if (listen(_serverFd, SOMAXCONN) < 0)
        return (std::cerr << "Error listen\n", false);

    std::cout << "Servidor escuchando en " << "10.14.8.3" << ":" << _port << std::endl;
    std::cout << "Presiona ENTER para salir.\n";
    std::cin.get();
    close(_serverFd);
    return (true);
}

int Server::getSocket(void) const 
{ 
    return (_serverFd); 
}

const std::string& Server::getIP(void) const 
{ 
    return (_ip); 
}

const std::string& Server::getPassword(void) const 
{ 
    return (_password); 
}
