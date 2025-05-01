#include "PollManager.hpp"
#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>


PollManager::PollManager(int serverFD, const std::string& password) : _password(password), _serverFD(serverFD)
{
    pollfd pfd;
    pfd.fd = _serverFD;
    pfd.events = POLLIN;
    _fds.push_back(pfd);
}

PollManager::~PollManager() {}

PollManager::PollManager(const PollManager& other) : _password(other._password), _serverFD(other._serverFD), _fds(other._fds) {}

PollManager& PollManager::operator=(const PollManager& other) 
{
    if (this != &other) 
    {
        _password = other._password;
        _serverFD = other._serverFD;
        _fds = other._fds;
    }
    return (*this);
}


const std::string& PollManager::getPassword() const         { return (_password); }

std::vector<pollfd>& PollManager::getFDs()                  { return (_fds); }

int PollManager::getServerFD() const                        {return (_serverFD); }

Client& PollManager::getClient(int fd)                      {return _clients.at(fd);}

std::map<std::string, Channel>& PollManager::getChannels()  {return _channels;}

std::map<int, Client>& PollManager::getClients()            {return _clients;}

void PollManager::removeClient(Client* client)
{
    int fd = client->getClientFD();
    client->setState(AWAITING_PASSWORD);
    std::map<std::string, Channel>& channels = getChannels();
    std::map<std::string, Channel>::iterator it;
    for (it = channels.begin(); it != channels.end(); ++it)
    {
        it->second.removeUser(client);
        it->second.removeAdmin(client);
    }
    std::vector<pollfd>& fds = getFDs();
    for (size_t i = 0; i < fds.size(); ++i)
    {
        if (fds[i].fd == fd)
        {
            fds.erase(fds.begin() + i);
            break;
        }
    }
    std::cout << YELLOW "[INFO] Cliente desconectado: " << fd << std::endl;
    close(fd);
    getClients().erase(fd);
}


void PollManager::acceptNewClient()
{
    sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int clientFD = accept(_serverFD, (sockaddr*)&clientAddr, &addrLen);
    if (clientFD < 0) 
    {
        std::cerr << "Error al aceptar cliente\n";
        return;
    }
    fcntl(clientFD, F_SETFL, O_NONBLOCK);
    Client newClient(clientFD);
    _clients[clientFD] = newClient;
    pollfd pfd;
    pfd.fd = clientFD;
    pfd.events = POLLIN;
    _fds.push_back(pfd);
    std::cout << GREEN "[SERVER] Cliente conectado en FD " << clientFD << "\n" RESET;
    send(clientFD, "\033[33mIntroduce la contraseña: \033[0m", 36, 0);
}


std::string PollManager::requestInput(int clientFD, const std::string& prompt)
{
    send(clientFD, prompt.c_str(), prompt.size(), 0);
    char buffer[512];
    int bytes = recv(clientFD, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) 
        return "";
    buffer[bytes] = '\0';
    std::string str(buffer);
    if (!str.empty() && str[str.length() - 1] == '\n')
        str.erase(str.length() - 1);
    if (!str.empty() && str[str.length() - 1] == '\r')
        str.erase(str.length() - 1);
    return (str);
}

void PollManager::run()
{
    std::cout << GREEN "[SERVER] Esperando conexiones...\n" RESET;
    _channels["General"] = Channel("General", "", false, false);
    while (true) 
    {
        int poll_count = poll(_fds.data(), _fds.size(), -1);
        if (poll_count < 0)
            throw std::runtime_error("poll() falló");

        for (size_t i = 0; i < _fds.size(); ++i) 
        {
            if (_fds[i].revents & POLLIN) 
            {
                if (_fds[i].fd == _serverFD)
                    acceptNewClient();
                else 
                {
                    Client& client = _clients[_fds[i].fd];
                    std::string input = requestInput(_fds[i].fd, "");
                    if (input.empty()) 
                        continue;
                    switch (client.getState()) 
                    {
                        case AWAITING_PASSWORD:
                            if (input != _password) 
                            {
                                close(_fds[i].fd);
                                _clients.erase(_fds[i].fd);
                                _fds.erase(_fds.begin() + i);
                                --i;
                            } 
                            else 
                            {
                                client.setState(AWAITING_NICKNAME);
                                input = requestInput(_fds[i].fd, "\033[33mIntroduce tu IRC nickname: \033[0m");
                            }
                            break;
                        case AWAITING_NICKNAME:
                        {
                            bool nicknameTaken = false;
                            for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
                            {
                                if (it->second.getNickname() == input)
                                {
                                    nicknameTaken = true;
                                    break;
                                }
                            }
                            if (nicknameTaken)
                            {
                                input = requestInput(_fds[i].fd, "\033[31mEse nickname ya está en uso. Introduce otro: \033[0m");
                            }
                            else
                            {
                                client.setNickname(input);
                                client.setState(AWAITING_USERNAME);
                                input = requestInput(_fds[i].fd, "\033[33mIntroduce tu IRC username: \033[0m");
                            }
                            break;
                        }
                        case AWAITING_USERNAME:
                        {
                            bool usernameTaken = false;
                            for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
                            {
                                if (it->second.getUsername() == input)
                                {
                                    usernameTaken = true;
                                    break;
                                }
                            }
                            if (usernameTaken)
                            {
                                input = requestInput(_fds[i].fd, "\033[31mEse username ya está en uso. Introduce otro: \033[0m");
                            }
                            else
                            {
                                client.setUsername(input);
                                client.setActualGroup("General");
                                _channels["General"].addUser(&client);
                                client.setState(READY);
                                send(_fds[i].fd, WELCOMEMSG, strlen(WELCOMEMSG), 0);
                            }
                            break;
                        }
                        case READY:
                            HandleInput(_fds[i].fd, *this);
                            break;
                    }
                }
            }
        }
    }
}

