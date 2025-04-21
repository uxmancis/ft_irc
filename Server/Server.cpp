#include "Server.hpp"

Server::Server(int port, const std::string& password) : _port(port), _serverFd(-1), _password(password) {}

Server::~Server() 
{
    if (_serverFd != -1)
        close(_serverFd);
}

std::string Server::_getLocalIP(void) 
{
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == -1) 
        return (std::cerr << "Error obteniendo hostname\n", "");
    hostent* host = gethostbyname(hostname);
    if (!host || !host->h_addr_list[0]) 
        return (std::cerr << "Error obteniendo IP local\n", "");
    return (inet_ntoa(*(struct in_addr*)host->h_addr_list[0]));
}

bool Server::setup(void) 
{
    _ip = _getLocalIP();
    if (_ip.empty()) 
        return false;

    _serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFd < 0) 
        return (std::cerr << "Error socket\n", false);


    int opt = 1;
    setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr; {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = inet_addr(_ip.c_str());

    if (bind(_serverFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
        return (std::cerr << "Error bind\n", false);

    if (listen(_serverFd, SOMAXCONN) < 0)
        return (std::cerr << "Error listen\n", false);

    std::cout << "Servidor escuchando en " << _ip << ":" << _port << std::endl;
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
