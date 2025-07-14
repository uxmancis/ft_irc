#include "PollManager.hpp"
#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include <csignal>
#include <cerrno>

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

const std::string& PollManager::getPassword() const { return (_password); }

std::vector<pollfd>& PollManager::getFDs() { return (_fds); }

int PollManager::getServerFD() const { return (_serverFD); }

Client& PollManager::getClient(int fd) { return _clients.at(fd); }

std::map<std::string, Channel>& PollManager::getChannels() { return _channels; }

std::map<int, Client>& PollManager::getClients() { return _clients; }

void PollManager::removeClient(Client* client)
{
    int fd = client->getClientFD();
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
    std::cout << YELLOW << "[INFO] Client disconnected: " << fd << std::endl << RESET;
    close(fd);
    _clients.erase(fd);
}

void PollManager::acceptNewClient()
{
    sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int clientFD = accept(_serverFD, (sockaddr*)&clientAddr, &addrLen);
    if (clientFD < 0)
    {
        std::cerr << "Error accepting client" << std::endl;
        return;
    }
    if (fcntl(clientFD, F_SETFL, O_NONBLOCK) < 0)
    {
        std::cerr << "Error setting client socket to non-blocking" << std::endl;
        close(clientFD);
        return;
    }
    Client newClient(clientFD);
    _clients[clientFD] = newClient;
    pollfd pfd;
    pfd.fd = clientFD;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _fds.push_back(pfd);
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
    std::cout << GREEN << "[SERVER] Waiting for connections..." << RESET << std::endl;
    _channels["#general"] = Channel("#general", "", false, false);

    extern volatile sig_atomic_t g_running;

    while (g_running)
    {
        int poll_count = poll(&_fds[0], _fds.size(), 1000);
        if (poll_count < 0)
        {
            if (errno == EINTR)
                continue;
            throw std::runtime_error("poll() failed");
        }
        for (size_t i = 0; i < _fds.size(); ++i)
        {
            if (_fds[i].revents & POLLIN)
            {
                if (_fds[i].fd == _serverFD)
                {
                    acceptNewClient();
                }
                else
                {
                    int fd = _fds[i].fd;
                    char buffer[512];
                    int bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
                    if (bytes <= 0)
                    {
                        removeClient(&_clients[fd]);
                        --i;
                        continue;
                    }
                    buffer[bytes] = '\0';
                    std::string input(buffer);
                    std::istringstream iss(input);
                    std::string line;
                    while (std::getline(iss, line))
                    {
                        if (!line.empty() && line[line.length() - 1] == '\r')
                            line.erase(line.length() - 1);
                        if (_clients.find(fd) == _clients.end())
                            break;
                        HandleCommands(fd, line, *this);
                        if (_clients.find(fd) == _clients.end())
                            break;
                        Client& client = _clients[fd];
                        if (!client.getNickname().empty() && !client.getUsername().empty() && client.getState() != READY)
                            client.setState(READY);
                        if (client.getState() == READY && !client.hasJoinedGeneral())
                        {
                            std::string welcome = ":irc.local 001 " + client.getNickname() + " :Welcome to the IRC server, " + client.getNickname() + "\r\n";
                            send(fd, welcome.c_str(), welcome.size(), 0);
                            std::vector<std::string> args;
                            args.push_back("#general");
                            HandleJOIN(fd, args, *this);
                            std::cout << GREEN << "[SERVER] Client connected on FD " << fd << std::endl << RESET;
                            client.setJoinedGeneral(true);
                        }
                    }
                }
            }
        }
    }
    std::cout << YELLOW << "[SERVER] Shutting down. Cleaning up resources..." << RESET << std::endl;
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        close(it->first);
    _clients.clear();
    _fds.clear();
    _channels.clear();
}
