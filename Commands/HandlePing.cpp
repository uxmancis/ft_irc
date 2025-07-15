#include "Commands.hpp"
#include <sstream>
#include <vector>


void HandlePING(int fd, const std::vector<std::string>& args)
{
    std::string token = args.empty() ? "irc.local" : args[0];
    std::string response = "PONG :" + token + "\r\n";
    send(fd, response.c_str(), response.size(), 0);
}

