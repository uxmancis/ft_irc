
#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <cstring>
#include <cstdlib>

#include "../PollManager/PollManager.hpp"

class Server 
{
    public:
        Server(int port, const std::string& password);
        ~Server(void);
    
    private:
        int         _serverFD;
        std::string _password;
        PollManager _pollManager;

    };

#endif