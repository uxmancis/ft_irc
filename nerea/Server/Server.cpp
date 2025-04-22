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
    
        //Reusar dirección inmediatamente tras cierre
    int opt = 1;
    setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); //SO_REUSEADDR evita "puertos ocupados" tras reinicios rápidos.
    
    //Poner descriptor en modo no bloqueante
    if (fcntl(_serverFd, F_SETFL, O_NONBLOCK) < 0) //accpt(), recv() y send() devuelvan inmediatamente
        return std::cerr << "Error fcntl\n", false; 
    
    //bind a cualquier interfaz (INADDR_ANY) y al puerto
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY); //addr.sin_addr.s_addr = inet_addr("10.14.8.3");


    if (bind(_serverFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
        return (std::cerr << "Error bind\n", false);

    //escuchamos pero no juzgamos
    if (listen(_serverFd, SOMAXCONN) < 0)
        return (std::cerr << "Error listen\n", false);

    sockaddr_in actual;
    socklen_t len = sizeof(actual);
    getsockname(_serverFd, (sockaddr*)&actual, &len);
    
    char buf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &actual.sin_addr, buf, sizeof(buf));
    std::cout << "Bind en: " << buf << ":" << ntohs(actual.sin_port) << "\n";
        
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
