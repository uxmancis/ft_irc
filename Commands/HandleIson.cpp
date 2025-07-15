#include "Commands.hpp"
#include <sstream>
#include <vector>
#include <cstdlib>

void HandleISON(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    Client& client = pollManager.getClient(fd);
    std::string nick = client.getNickname();

    if (args.empty())
    {
        std::string err = ":irc.local 461 " + nick + " ISON :Not enough parameters\r\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }

    const std::map<int, Client>& clients = pollManager.getClients();
    std::string onlineNicks;
    std::string rawNicks;
    for (size_t i = 0; i < args.size(); ++i)
    {
        if (i > 0) rawNicks += " ";
        rawNicks += args[i];
    }

    std::istringstream iss(rawNicks);
    std::string nickToCheck;
    while (iss >> nickToCheck)
    {
        for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it)
        {
            if (it->second.getNickname() == nickToCheck)
            {
                if (!onlineNicks.empty())
                    onlineNicks += " ";
                onlineNicks += nickToCheck;
                break;
            }
        }
    }

    std::string reply = ":irc.local 303 " + nick + " :" + onlineNicks + "\r\n";
    send(fd, reply.c_str(), reply.size(), 0);
}
