#include "ClientManager.hpp"

ClientManager::ClientManager(int clientfd)
{
    pollfd pfd;
    pfd.fd = clientfd;
    pfd.events = POLLIN;
    _clients.push_back(pfd);

    send(clientfd, "Introduce tu nombre de usuario (max 12 chars): ", 48, 0);
    char   buffer[13];
    int bytes = recv(clientfd, buffer, sizeof(buffer), 0);
    buffer[bytes] = '\0';
    std::string pass  = buffer;
    pass = _name;
}