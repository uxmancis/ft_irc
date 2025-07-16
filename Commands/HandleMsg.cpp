#include "Commands.hpp"
#include <sstream>
#include <vector>
#include <map>

void Commands::handleMSG(int fd, const std::vector<std::string> &args, PollManager &pollManager)
{
    if (args.size() < 2)
    {
        std::string err = ":" + pollManager.getHostname() + " 461 * PRIVMSG :Not enough parameters\r\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }

    Client &sender = pollManager.getClient(fd);
    std::string msg;
    size_t start = (args[1][0] == ':') ? 1 : 0;

    for (size_t i = 1; i < args.size(); ++i)
    {
        if (i == start && args[i][0] == ':')
            msg += args[i].substr(1);
        else
            msg += " " + args[i];
    }

    const std::string &target = args[0];
    std::string prefix = ":" + sender.getNickname() + "!" + sender.getUsername() + "@" + pollManager.getHostname() + " PRIVMSG ";
    std::string finalMsg = prefix + target + " :" + msg + "\r\n";

    if (target[0] == '#')
    {
        std::map<std::string, Channel> &channels = pollManager.getChannels();
        if (channels.find(target) == channels.end() || !channels[target].hasUser(&sender))
        {
            std::string err = ":" + pollManager.getHostname() + " 403 " + target + " :No such channel\r\n";
            send(fd, err.c_str(), err.size(), 0);
            return;
        }

        const std::vector<Client *> &users = channels[target].getRegularUsers();
        std::vector<Client *>::const_iterator it;
        for (it = users.begin(); it != users.end(); ++it)
        {
            if ((*it)->getClientFD() != fd)
                send((*it)->getClientFD(), finalMsg.c_str(), finalMsg.size(), 0);
        }
    }
    else
    {
        std::map<int, Client> &clients = pollManager.getClients();
        std::map<int, Client>::iterator it;
        for (it = clients.begin(); it != clients.end(); ++it)
        {
            if (it->second.getNickname() == target)
            {
                send(it->first, finalMsg.c_str(), finalMsg.size(), 0);
                return;
            }
        }

        std::string err = ":" + pollManager.getHostname() + " 401 " + target + " :No such nick\r\n";
        send(fd, err.c_str(), err.size(), 0);
    }
}
