#include "Commands.hpp"
#include <sstream>
#include <vector>


void HandlePONG(int fd, PollManager &pollManager)
{
    std::cout << RED "RECEIVED PONG [fd = " << fd << "] " RESET << std::endl;
   pollManager.getClient(fd)._receivedPONG = true;
}
