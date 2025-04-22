#ifndef POLLMANAGER_HPP
# define POLLMANAGER_HPP

#include <vector>
#include <poll.h>
#include <string>

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