#include "PollManager.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/socket.h>

PollManager::PollManager(int serverFD, std::string password) : _password(password), _serverFD(serverFD)
{
    pollfd pfd;
    pfd.fd = _serverFD;
    pfd.events = POLLIN;
    _fds.push_back(pfd);
}

void PollManager::AcceptNewUser(void)
{
    int client_fd = accept(_serverFD, NULL, NULL);
    send(client_fd, "Introduce la contraseña: ", 27, 0);
    char   buffer[_password.length() + 1];
    int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    buffer[bytes] = '\0';
    std::string pass  = buffer;
    if (pass != _password)
    {
        send(client_fd, "Contraseña incorrecta\n", 24, 0);
        close(client_fd);
        return ;
    }
    send(client_fd, "Hola", 5, 0);
}

void PollManager::run(void)
{
    std::cout << "Esperando conexiones...\n";
    while (true) 
    {
        int n = poll(_fds.data(), _fds.size(), -1);
        if (n < 0) 
        {
            std::cerr << "Poll error\n";
            break;
        }
        AcceptNewUser();
    }
}