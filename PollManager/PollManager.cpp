#include "PollManager.hpp"

void PollManager::addFD(int fd, short events) 
{
    pollfd pfd = {fd, events, 0};
    _fds.push_back(pfd);
}