#ifndef POLLMANAGER_HPP
# define POLLMANAGER_HPP

#include "../ClientManager/ClientManager.hpp"

#include <vector>
#include <poll.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

class PollManager 
{
    public:
        PollManager(int serverFD, std::string password);

        void    AcceptNewUser(void);
        void    run(void);

    private:
        std::string         _password;
        int                 _serverFD;
        std::vector<pollfd> _fds;

};

#endif