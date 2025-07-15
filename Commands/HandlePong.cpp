#include "Commands.hpp"
#include <sstream>
#include <vector>
#include <algorithm> 

void HandlePONG(int fd)
{
    std::cout << "[DEBUG] Received PONG from client FD " << fd << std::endl;
}
