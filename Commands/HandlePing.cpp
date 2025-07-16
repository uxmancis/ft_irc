#include "Commands.hpp"


void Commands::handlePING(int fd, const std::vector<std::string> &args, PollManager &pollManager)
{
    if (args.empty())
    {
        std::string send_ping = ":" + pollManager.getHostname() + " PING " + pollManager.getHostname() + " :" + pollManager.getHostname() + "\r\n";
        send(fd, send_ping.c_str(), send_ping.size(), 0);
        pollManager.getClient(fd).setPreviousPING(time(NULL)); // apuntamos la hora de este ping
        // std::cout <<  MAGENTA "PING SENT [fd = " << fd << "]:" RESET << pollManager.getClient(fd)._previousPING << std::endl;
        return;
    }
    std::string origin = args[0];
    std::string response = ":" + pollManager.getHostname() + " PONG " + pollManager.getHostname() + " :" + origin + "\r\n";
    send(fd, response.c_str(), response.size(), 0);
    /* std::cout << "[DEBUG] Responded to PING with: " << response; */
}
