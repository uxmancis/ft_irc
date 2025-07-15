#include "Commands.hpp"
#include <sstream>
#include <vector>
#include <algorithm> 

void HandlePONG(int fd, const std::vector<std::string>& args)
{
    if (args.empty())
    {
        std::string error = ":irc.local 409 * :No origin specified\r\n";
        send(fd, error.c_str(), error.size(), 0);
        return;
    }

    std::string token = args[0];
    std::string response = "PING " + token + "\r\n";
    send(fd, response.c_str(), response.size(), 0);
}