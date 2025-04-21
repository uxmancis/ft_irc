#ifndef POLLMANAGER_HPP
# define POLLMANAGER_HPP

#include <vector>
#include <poll.h>

class PollManager 
{
    public:
        PollManager(int serverFD);

    private:
        int                 _serverFD;
        std::vector<pollfd> _fds;

};

#endif