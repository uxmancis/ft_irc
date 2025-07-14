#include "Commands.hpp"
#include <sstream>
#include <vector>

void HandlePART(int fd, const std::vector<std::string>& args, PollManager& pollManager) 
{
    if (args.empty()) 
    {
        std::string err = ":irc.local 461 * PART :Not enough parameters\r\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }

    std::string channelName = args[0];
    std::map<std::string, Channel>& channels = pollManager.getChannels();
    Client& client = pollManager.getClient(fd);

    if (channels.find(channelName) == channels.end()) 
    {
        std::string err = ":irc.local 403 " + client.getNickname() + " " + channelName + " :No such channel\r\n"; // ERR_NOSUCHCHANNEL
        send(fd, err.c_str(), err.size(), 0);
        return;
    }

    Channel& channel = channels[channelName];

    if (!channel.hasUser(&client)) {
        std::string err = ":irc.local 442 " + client.getNickname() + " " + channelName + " :You're not on that channel\r\n"; // ERR_NOTONCHANNEL
        send(fd, err.c_str(), err.size(), 0);
        return;
    }

    std::string partMsg = (args.size() >= 2) ? args[1] : client.getNickname();

    std::string msg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost PART " + channelName + " :" + partMsg + "\r\n";

    std::vector<Client*> users = channel.getRegularUsers();
    for (size_t i = 0; i < users.size(); ++i)
        send(users[i]->getClientFD(), msg.c_str(), msg.size(), 0);

    channel.removeUser(&client);
    channel.removeAdmin(&client);

    if (channel.getRegularUsers().empty() && channel.getAdmins().empty()) 
        channels.erase(channelName);

    if (client.getActualGroup() == channelName)
        client.setActualGroup("");
}
