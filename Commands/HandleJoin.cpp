#include "Commands.hpp"
#include <sstream>
#include <vector>
#include <cstdlib>
#include <ctime>

void Commands::handleJOIN(int fd, const std::vector<std::string> &args, PollManager &pollManager)
{
    if (args.empty())
        return;

    Client &client = pollManager.getClient(fd);
    std::string channelName = args[0];

    if (channelName[0] != '#')
        channelName = "#" + channelName;

    std::map<std::string, Channel> &channels = pollManager.getChannels();
    bool isNew = false;

    if (channels.find(channelName) == channels.end())
        isNew = true;

    Channel &channel = channels[channelName];

    if (channel.isInviteOnly())
    {
        std::string errMsg = ":" + pollManager.getHostname() + " 473 " + client.getNickname() + " " + channelName + " :Cannot join channel (+i)\r\n";
        send(fd, errMsg.c_str(), errMsg.size(), 0);
        return;
    }

    if (!channel.getPassword().empty())
    {
        if (args.size() < 2 || args[1] != channel.getPassword())
        {
            std::string errMsg = ":" + pollManager.getHostname() + " 475 " + client.getNickname() + " " + channelName + " :Cannot join channel (+k)\r\n";
            send(fd, errMsg.c_str(), errMsg.size(), 0);
            return;
        }
    }

    if (channel.isFull())
    {
        std::string errMsg = ":" + pollManager.getHostname() + " 471 " + client.getNickname() + " " + channelName + " :Cannot join channel (+l)\r\n";
        send(fd, errMsg.c_str(), errMsg.size(), 0);
        return;
    }

    channel.addUser(&client);
    if (isNew)
        channel.addAdmin(&client);

    client.setActualGroup(channelName);

    std::string joinMsg = ":" + client.getNickname() + "!user@" + pollManager.getHostname() + " JOIN " + channelName + "\r\n";

    const std::vector<Client *> &users = channel.getRegularUsers();
    std::vector<Client *>::const_iterator uit;
    for (uit = users.begin(); uit != users.end(); ++uit)
        send((*uit)->getClientFD(), joinMsg.c_str(), joinMsg.size(), 0);

    std::string usersList;
    for (uit = users.begin(); uit != users.end(); ++uit)
    {
        if (channel.isAdmin(*uit))
            usersList += "@" + (*uit)->getNickname() + " ";
        else
            usersList += (*uit)->getNickname() + " ";
    }

    std::string namesMsg = ":" + pollManager.getHostname() + " 353 " + client.getNickname() + " = " + channelName + " :" + usersList + "\r\n";
    std::string endNamesMsg = ":" + pollManager.getHostname() + " 366 " + client.getNickname() + " " + channelName + " :End of /NAMES list.\r\n";
    send(fd, namesMsg.c_str(), namesMsg.size(), 0);
    send(fd, endNamesMsg.c_str(), endNamesMsg.size(), 0);

    if (!channel.getTopic().empty())
    {
        std::stringstream ss;
        ss << time(NULL);
        std::string topicMsg = ":" + pollManager.getHostname() + " 332 " + client.getNickname() + " " + channelName + " :" + channel.getTopic() + "\r\n";
        std::string topicSetMsg = ":" + pollManager.getHostname() + " 333 " + client.getNickname() + " " + channelName + " " + client.getNickname() + " " + ss.str() + "\r\n";
        send(fd, topicMsg.c_str(), topicMsg.size(), 0);
        send(fd, topicSetMsg.c_str(), topicSetMsg.size(), 0);
    }
}
