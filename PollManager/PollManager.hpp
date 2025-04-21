#ifndef POLLMANAGER_HPP
# define POLLMANAGER_HPP

#include <vector>
#include <poll.h>
#include <map>

class PollManager 
{
    public:
        void addFD(int fd, short events);

    private:
        std::vector<pollfd> _fds;
    
    };
    

#endif