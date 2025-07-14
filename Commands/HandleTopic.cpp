#include "Commands.hpp"
#include <sstream>
#include <vector>
#include <map>

void HandleTOPIC(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    if (args.empty())
    {
        std::string err = ":irc.local 461 TOPIC :Not enough parameters\r\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    Client& client = pollManager.getClient(fd);
    std::string channelName = args[0];
    std::map<std::string, Channel>& channels = pollManager.getChannels();
    if (channels.count(channelName) == 0)
        return;
    Channel& channel = channels[channelName];
    if (args.size() == 1)
    {
        std::string topic = channel.getTopic();
        if (topic.empty())
        {
            std::string msg = ":irc.local 331 " + client.getNickname() + " " + channelName + " :No topic is set\r\n";
            send(fd, msg.c_str(), msg.size(), 0);
        }
        else
        {
            std::string msg = ":irc.local 332 " + client.getNickname() + " " + channelName + " :" + topic + "\r\n";
            send(fd, msg.c_str(), msg.size(), 0);
        }
        return;
    }
    if (!channel.isFreeTopic() && !channel.isAdmin(&client))
    {
        std::string err = ":irc.local 482 " + channelName + " :You're not a channel operator\r\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    std::string newTopic;
    for (size_t i = 1; i < args.size(); ++i)
    {
        newTopic += args[i];
        if (i + 1 < args.size())
            newTopic += " ";
    }
    channel.setChannelTopic(newTopic);
    std::string broadcast = ":" + client.getNickname() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
    const std::vector<Client*>& users = channel.getRegularUsers();
    for (std::vector<Client*>::const_iterator it = users.begin(); it != users.end(); ++it)
    {
        send((*it)->getClientFD(), broadcast.c_str(), broadcast.size(), 0);
    }
}









