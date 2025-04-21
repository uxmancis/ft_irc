
#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

class Server {
    public:
        Server(int port, const std::string& password);
        ~Server(void);
    
        bool                setup(void);
        int                 getSocket(void) const;
        const std::string&  getIP(void) const;
        const std::string&  getPassword(void) const;
    
    private:
        int         _port;
        int         _serverFd;
        std::string _ip;
        std::string _password;
    
        std::string _getLocalIP(void);
    };

#endif