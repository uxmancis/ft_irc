#include "Commands.hpp"
#include <sstream>
#include <vector>


void HandlePING(int fd, const std::vector<std::string>& args, PollManager &pollManager)
{
    if (args.empty())
    {
        std::string send_ping = ":irc.local PING irc.local :irc.local\r\n";
        send(fd, send_ping.c_str(), send_ping.size(), 0);
        pollManager.getClient(fd)._previousPING = time(NULL); // apuntamos la hora de este ping
        std::cout <<  MAGENTA "PING SENT [fd = " << fd << "]:" RESET << pollManager.getClient(fd)._previousPING << std::endl;
        return;
    }
    std::string origin = args[0];
    std::string response = ":irc.local PONG irc.local :" + origin + "\r\n";
    send(fd, response.c_str(), response.size(), 0);
    /* std::cout << "[DEBUG] Responded to PING with: " << response; */
}
