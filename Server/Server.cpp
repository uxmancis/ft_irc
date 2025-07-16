#include "Server.hpp"

Server::Server(int port, const std::string &password) : _serverFD(-1), _password(password), _port(port)
{
    _setupSocket();
}

Server::~Server()
{
    if (_serverFD != -1)
        close(_serverFD);
}

Server::Server(const Server &other) : _serverFD(-1), _password(other._password), _port(other._port)
{
    _setupSocket();
}

Server &Server::operator=(const Server &other)
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
        throw std::runtime_error("Error creating socket: " + std::string(strerror(errno)));
    int opt = 1;
    if (setsockopt(_serverFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw std::runtime_error("Error setting socket options: " + std::string(strerror(errno)));
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(_serverFD, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        throw std::runtime_error("Error binding socket: " + std::string(strerror(errno)));
    if (listen(_serverFD, SOMAXCONN) < 0)
        throw std::runtime_error("Error listening on socket: " + std::string(strerror(errno)));
    if (fcntl(_serverFD, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("Error setting non-blocking mode: " + std::string(strerror(errno)));
}

void Server::run()
{
    std::cout << "Server started on port " << _port << std::endl;
    char hostname[MAX_HOSTNAME_LENGTH];
    std::string host;
    if (gethostname(hostname, sizeof(hostname)) == 0)
    {
        hostname[MAX_HOSTNAME_LENGTH - 1] = '\0';
        host = hostname;
    }
    else
        host = "irc.local";
    PollManager pollManager(_serverFD, _password, host);
    pollManager.run();
}
