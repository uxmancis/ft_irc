#include "PollManager.hpp"
#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>

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
/*     getClients().erase(fd); */
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
    /* std::cout << GREEN "[SERVER] Cliente conectado en FD " << clientFD << "\n" RESET; */
    
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

/* void PollManager::run()
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
} */


void PollManager::run()
{
    std::cout << GREEN "[SERVER] Esperando conexiones...\n" RESET;
    _channels["#general"] = Channel("#general", "", false, false);

/*     int test;

    test = 4; */
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
                {
                    acceptNewClient();
                }
                else 
                {
                    Client& client = _clients[_fds[i].fd];
                    char buffer[512];
                    int bytes = recv(_fds[i].fd, buffer, sizeof(buffer) - 1, 0);

                    if (bytes <= 0) 
                    {
                        removeClient(&client);
                        --i;
                        continue;
                    }

                    buffer[bytes] = '\0';
                    std::string input(buffer);
                    std::istringstream iss(input);
                    std::string line;
                    while (std::getline(iss, line)) 
                    {
                        if (!line.empty() && line.back() == '\r')
                            line.pop_back();

                        std::istringstream lineStream(line);
                        std::string command;
                        lineStream >> command;

                        if (command == "PASS") 
                        {
                            std::string pass;
                            lineStream >> pass;
                            client.setPassword(pass);
                        }
                        else if (command == "NICK") 
                        {
                            std::string nick;
                            lineStream >> nick;

                            // Comprobar si nickname está en uso
                            bool taken = false;
                            std::map<int, Client>::iterator it;
                            for (it = _clients.begin(); it != _clients.end(); ++it)
                            {
                              
                                Client& c = it->second;
                                if (c.getNickname() == nick && c.getClientFD() != client.getClientFD()) 
                                {
                                    taken = true;
                                    break;
                                }
                            }
                            if (taken) 
                            {
                                std::string msg = ":irc.local 433 * " + nick + " :Nickname is already in use\r\n";
                                send(client.getClientFD(), msg.c_str(), msg.size(), 0);
                            } 
                            else 
                            {
                                client.setNickname(nick);
                            }
                        }
                        else if (command == "USER") 
                        {
                            std::string username, unused1, unused2, realname;
                            lineStream >> username >> unused1 >> unused2;
                            std::getline(lineStream, realname);
                            if (!realname.empty() && realname[0] == ':')
                                realname = realname.substr(1);

                            client.setUsername(username);
                            // Puedes almacenar realname si quieres
                        }

                        // Si ya tiene PASS, NICK y USER configurados, verifica la contraseña y da la bienvenida
                        if (client.getPassword() != "" && client.getNickname() != "" && client.getUsername() != "" && client.getState() != READY) 
                        {
                            if (client.getPassword() != _password) 
                            {
                                std::string msg = ":irc.local 464 * :Password incorrect\r\n";
                                send(client.getClientFD(), msg.c_str(), msg.size(), 0);
                                removeClient(&client);
                                --i;
                                break;
                            }
                            
                            client.setState(READY);
                            std::cout << GREEN "[SERVER] Cliente conectado en FD " << client.getClientFD() << "\n" RESET;
                        
                            // Añadir cliente al canal general
                            client.setActualGroup("#general");
                            _channels["#general"].addUser(&client);
                        
                            std::string nick = client.getNickname();
                            int fd = client.getClientFD();
                        
                            // Mensajes para que el cliente "entre" al canal automáticamente
                            std::string welcome = 
                                ":irc.local 001 " + nick + " :Welcome to the IRC network\r\n"
                                ":irc.local 002 " + nick + " :Your host is irc.local\r\n"
                                ":irc.local 332 " + nick + " #general :Canal General\r\n"  // Topic del canal
                                ":irc.local 353 " + nick + " = #general :" + nick + "\r\n"  // Nombres en el canal (solo el propio)
                                ":irc.local 366 " + nick + " #general :End of /NAMES list.\r\n"; // Fin lista de usuarios
                        
                            send(fd, welcome.c_str(), welcome.size(), 0);
                        }

                        // Si ya está listo, procesa comandos IRC normales (ej. JOIN, PRIVMSG)
                        if (client.getState() == READY) 
                        {
                          /*   std::string response;
                            response += ":" + client.getNickname() + "!" + client.getUsername() + "@" + "host" + " JOIN :" + "#general" + "\r\n";    
                            if (test == client.getClientFD())
                                send(client.getClientFD(), response.c_str(), response.size(), 0);
                            test++; */
                            HandleInput(client.getClientFD(), *this, line);
                        }
                    }
                }
            }
        }
    }
}
