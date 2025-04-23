#ifndef CLIENTMANAGER_HPP
# define CLIENTMANAGER_HPP

#include <vector>
#include <poll.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/socket.h>

class ClientManager 
{
    public:
        ClientManager(int clienfd);


    private:
        std::vector<pollfd> _clients;
        std::string         _name;

};

#endif