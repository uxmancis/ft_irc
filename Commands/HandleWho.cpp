#include "Commands.hpp"
#include <sstream>
#include <vector>

void Commands::handleWHO(int fd, const std::vector<std::string> &args, PollManager &pollManager)
{
    Client &requester = pollManager.getClient(fd);
    std::string requesterNick = requester.getNickname();

    if (args.empty())
    {
        std::string err = ":" + pollManager.getHostname() + " 461 " + requesterNick + " WHO :Not enough parameters\r\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }

    std::string target = args[0];
    std::map<std::string, Channel> &channels = pollManager.getChannels();
    std::map<int, Client> &allClients = pollManager.getClients();

    if (target[0] == '#')
    {
        if (channels.count(target) == 0)
        {
            std::string endMsg = ":" + pollManager.getHostname() + " 315 " + requesterNick + " " + target + " :End of /WHO list\r\n";
            send(fd, endMsg.c_str(), endMsg.size(), 0);
            return;
        }

        Channel &channel = channels[target];
        const std::vector<Client *> &users = channel.getRegularUsers();

        std::vector<Client *>::const_iterator it;
        for (it = users.begin(); it != users.end(); ++it)
        {
            Client *user = *it;
            std::string msg = ":" + pollManager.getHostname() + " 352 " + requesterNick + " " + target + " " +
                              user->getUsername() + " " + pollManager.getHostname() + " " + pollManager.getHostname() + " " +
                              user->getNickname() + " H :0 " + user->getUsername() + "\r\n";
            send(fd, msg.c_str(), msg.size(), 0);
        }

        std::string endMsg = ":" + pollManager.getHostname() + " 315 " + requesterNick + " " + target + " :End of /WHO list\r\n";
        send(fd, endMsg.c_str(), endMsg.size(), 0);
    }
    else
    {
        std::map<int, Client>::iterator it;
        for (it = allClients.begin(); it != allClients.end(); ++it)
        {
            Client &user = it->second;
            if (user.getNickname() == target)
            {
                std::string msg = ":" + pollManager.getHostname() + " 352 " + requesterNick + " * " +
                                  user.getUsername() + " localhost " + pollManager.getHostname() + " " +
                                  user.getNickname() + " H :0 " + user.getUsername() + "\r\n";
                send(fd, msg.c_str(), msg.size(), 0);
                break;
            }
        }
        std::string endMsg = ":" + pollManager.getHostname() + " 315 " + requesterNick + " " + target + " :End of /WHO list\r\n";
        send(fd, endMsg.c_str(), endMsg.size(), 0);
    }
}
