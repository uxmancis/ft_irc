#include "Commands.hpp"
#include <sstream>
#include <vector>
#include <algorithm> 

void HandlePONG(int fd) 
{
    std::cout << GREEN "[PONG recibido de cliente " << fd << "]" RESET << std::endl;
}
