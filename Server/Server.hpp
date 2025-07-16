#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <iostream>
# include <stdexcept>
# include <fcntl.h>
# include <unistd.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <string.h>
# include <errno.h>
# include <cstring>

# include "../Poll/PollManager.hpp"

# define MAX_HOSTNAME_LENGTH 256

class Server 
{
    public:
        Server(int port, const std::string& password);
        ~Server();
        Server(const Server& other);
        Server& operator=(const Server& other);

        void            run();

    private:
        int             _serverFD;
        std::string     _password;
        int             _port;
        void            _setupSocket();
};

#endif
