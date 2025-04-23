#ifndef POLLMANAGER_HPP
# define POLLMANAGER_HPP

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

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

class PollManager 
{
    public:
        PollManager(int serverFD, std::string password);

        void    AcceptNewUser(void);
        void    HandleNewMsg(int fd);
        void    run(void);

    private:
        std::string         _password;
        int                 _serverFD;
        std::vector<pollfd> _fds;

};

#endif