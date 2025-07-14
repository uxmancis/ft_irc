#include "Commands.hpp"
#include <sstream>
#include <vector>
#include <map>

void HandleKICK(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    if (args.size() < 2)
    {
        std::string err = ":irc.local 461 KICK :Usage: /KICK <#channel> <nickname> [comment]\r\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }

    const std::string& channelName = args[0];
    const std::string& targetNick = args[1];
    std::string comment = "Kicked by operator";

    if (args.size() > 2)
        comment = args[2];

    Client& sender = pollManager.getClient(fd);
    std::map<std::string, Channel>& channels = pollManager.getChannels();

    if (channels.count(channelName) == 0)
    {
        std::string err = ":irc.local 403 " + channelName + " :No such channel\r\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }

    Channel& channel = channels[channelName];

    if (!channel.isAdmin(&sender))
    {
        std::string err = ":irc.local 482 " + channelName + " :You're not a channel operator\r\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }

    Client* target = NULL;
    std::map<int, Client>& clients = pollManager.getClients();
    std::map<int, Client>::iterator it;
    for (it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second.getNickname() == targetNick)
        {
            target = &(it->second);
            break;
        }
    }

    if (target == NULL || target->getActualGroup() != channelName)
    {
        std::string err = ":irc.local 441 " + targetNick + " " + channelName + " :User is not in the channel\r\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }

    std::string kickMsg = ":" + sender.getNickname() + "!user@localhost KICK " + channelName + " " + targetNick + " :" + comment + "\r\n";

    const std::vector<Client*>& users = channel.getRegularUsers();
    std::vector<Client*>::const_iterator uit;
    for (uit = users.begin(); uit != users.end(); ++uit)
    {
        send((*uit)->getClientFD(), kickMsg.c_str(), kickMsg.size(), 0);
    }

    channel.removeUser(target);
    channel.removeAdmin(target);
    target->setActualGroup("General");
}
