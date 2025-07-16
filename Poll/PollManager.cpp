#include "PollManager.hpp"

PollManager::PollManager(int serverFD, const std::string &password, const std::string &host) : _serverFD(serverFD), _password(password), _hostname(host)
{
    pollfd pfd;
    pfd.fd = _serverFD;
    pfd.events = POLLIN;
    _fds.push_back(pfd);
}

PollManager::PollManager(const PollManager &other) : _serverFD(other._serverFD), _password(other._password), _hostname(other._hostname) {}

PollManager &PollManager::operator=(const PollManager &other)
{
    if (this != &other)
    {
        _serverFD = other._serverFD;
        _password = other._password;
        _hostname = other._hostname;
    }
    return (*this);
}

PollManager::~PollManager() {}

const std::string &PollManager::getPassword() const { return (_password); }
const std::string &PollManager::getHostname() const { return (_hostname); }

std::vector<pollfd> &PollManager::getFDs() { return (_fds); }

int PollManager::getServerFD() const { return (_serverFD); }

Client &PollManager::getClient(int fd) { return _clients.at(fd); }

std::map<std::string, Channel> &PollManager::getChannels() { return _channels; }

std::map<int, Client> &PollManager::getClients() { return _clients; }

void PollManager::removeClient(Client *client)
{
    int fd = client->getClientFD();
    std::map<std::string, Channel> &channels = getChannels();
    std::map<std::string, Channel>::iterator it;
    for (it = channels.begin(); it != channels.end(); ++it)
    {
        it->second.removeUser(client);
        it->second.removeAdmin(client);
    }
    std::vector<pollfd> &fds = getFDs();
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
    int clientFD = accept(_serverFD, (sockaddr *)&clientAddr, &addrLen);
    if (clientFD < 0)
    {
        std::cerr << RED << "[ERROR] Failed to accept client: " << strerror(errno) << RESET << std::endl;
        return;
    }
    if (fcntl(clientFD, F_SETFL, O_NONBLOCK) < 0)
    {
        std::cerr << RED << "[ERROR] Failed setting client socket to non-blocking: " << strerror(errno) << RESET << std::endl;
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

void PollManager::run()
{
    std::cout << GREEN << "[SERVER] Waiting for connections..." << RESET << std::endl;
    _channels["#general"] = Channel("#general", "", false, false);
    Commands commands;
    extern volatile sig_atomic_t g_running;
    std::map<int, std::string> inputBuffers; // buffer parcial por fd
    while (g_running)
    {
        int poll_count = poll(&_fds[0], _fds.size(), 1000);
        if (poll_count < 0)
        {
            if (errno == EINTR)
                continue;
            throw std::runtime_error("Poll failed: " + std::string(strerror(errno)));
        }
        // std::cout << CYAN "----------- This are current fds: --------------- " RESET << std::endl;
        for (size_t i = 0; i < _fds.size(); ++i)
        {
            if (_fds[i].revents & POLLIN)
            {
                // std::cout << _fds[i].fd << std::endl;
                if (_fds[i].fd == _serverFD)
                {
                    acceptNewClient();
                }
                else
                {
                    int fd = _fds[i].fd;
                    char buffer[BUFFER_SIZE];
                    int bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
                    if (bytes <= 0)
                    {
                        removeClient(&_clients[fd]);
                        inputBuffers.erase(fd);
                        --i;
                        continue;
                    }

                    buffer[bytes] = '\0';
                    inputBuffers[fd] += buffer;

                    std::string &buf = inputBuffers[fd];
                    size_t pos;
                    while ((pos = buf.find("\r\n")) != std::string::npos)
                    {
                        std::string line = buf.substr(0, pos);
                        buf.erase(0, pos + 2);
                        if (_clients.find(fd) == _clients.end())
                            break;
                        commands.handleCommand(fd, line, *this);
                        if (_clients.find(fd) == _clients.end())
                            break;
                        Client &client = _clients[fd];
                        if (!client.getNickname().empty() && !client.getUsername().empty() && client.getState() != READY)
                            client.setState(READY);
                        if (client.getState() == READY && !client.hasJoinedGeneral())
                        {
                            commands.handleMOTD(fd, *this);
                            std::vector<std::string> args;
                            args.push_back("#general");
                            commands.handleJOIN(fd, args, *this);
                            std::cout << GREEN << "[SERVER] Client connected on FD " << fd << std::endl << RESET;
                            client.setJoinedGeneral(true);
                        }
                    }
                }
            }
            std::map<int, Client> &clients = this->getClients();
            std::map<int, Client>::iterator it;
            std::vector<std::string> args2;
            time_t current_time = time(NULL);
            for (it = clients.begin(); it != clients.end(); ++it)
            {
                if (it->second.getPreviousPING() == -1) // Si nunca antes hemos hecho un PING
                {
                    commands.handlePING(it->second.getClientFD(), args2, *this); // Hagamos PING por 1era vez y apuntamos hora it->second._previousPING
                    // std::cout << YELLOW << "1st PING!!! [fd = " << it->second.getClientFD() << "] " RESET << std::endl;
                }
                else
                {
                    time_t diff = current_time - it->second.getPreviousPING();
                    if (diff >= PING_TIMEOUT) // 10 refleja 10 segundos de diferencia que sí permitimos. Si sí han pasado 10 segundos, vuelvo a hacer lo mío...
                    {
                        // std::cout << CYAN "POLICÍA [fd = " << it->second.getClientFD() << "] " RESET << current_time << std::endl;
                        if (it->second.hasReceivedPONG() == false) // #1 si en este tiempo no he recibido PONG, agur --> HandleQUIT
                        {
                            commands.handleQUIT(it->second.getClientFD(), *this);
                            break;
                        }
                        else
                        {
                            it->second.setReceivedPONG(false);
                            commands.handlePING(it->second.getClientFD(), args2, *this);
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
