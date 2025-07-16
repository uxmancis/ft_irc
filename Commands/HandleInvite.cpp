#include "Commands.hpp"

void Commands::handleINVITE(int fd, const std::vector<std::string> &args, PollManager &pollManager)
{
    if (args.size() < 1)
    {
        std::string err = ":" + pollManager.getHostname() + " 461 INVITE :Usage: /INVITE <nickname>\r\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }

    Client &sender = pollManager.getClient(fd);
    const std::string &channelName = args[1];

    std::map<std::string, Channel> &channels = pollManager.getChannels();

    if (channels.count(channelName) == 0)
        return;

    Channel &channel = channels[channelName];

    if (!channel.isAdmin(&sender))
    {
        std::string err = ":" + pollManager.getHostname() + " 482 " + channelName + " :You're not a channel operator\r\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }

    Client *invited = NULL;
    std::map<int, Client> &clients = pollManager.getClients();
    std::map<int, Client>::iterator it;
    for (it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second.getNickname() == args[0])
        {
            invited = &it->second;
            break;
        }
    }

    if (!invited)
    {
        std::string err = ":" + pollManager.getHostname() + " 401 " + args[0] + " :No such nick\r\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }

    int limit = std::atoi(channel.getLimit().c_str());
    if (limit > 0 && (int)(channel.getRegularUsers().size()) >= limit)
    {
        std::string msg = ":" + pollManager.getHostname() + " 471 " + channelName + " :Channel is full\r\n";
        send(fd, msg.c_str(), msg.size(), 0);
        return;
    }

    invited->setActualGroup(channelName);
    channel.addUser(invited);

    std::string msgToSender = ":" + pollManager.getHostname() + " 341 " + sender.getNickname() + " " + invited->getNickname() + " " + channelName + "\r\n";
    send(fd, msgToSender.c_str(), msgToSender.size(), 0);

    std::string msgToInvited = ":" + sender.getNickname() + "!user@" + pollManager.getHostname() + " INVITE " + invited->getNickname() + " :" + channelName + "\r\n";
    send(invited->getClientFD(), msgToInvited.c_str(), msgToInvited.size(), 0);

    std::string joinMsg = ":" + invited->getNickname() + "!user@" + pollManager.getHostname() + " JOIN " + channelName + "\r\n";
    const std::vector<Client *> &users = channel.getRegularUsers();
    std::vector<Client *>::const_iterator uit;
    for (uit = users.begin(); uit != users.end(); ++uit)
    {
        send((*uit)->getClientFD(), joinMsg.c_str(), joinMsg.size(), 0);
    }

    std::string usersList;
    for (uit = users.begin(); uit != users.end(); ++uit)
    {
        if (channel.isAdmin(*uit))
            usersList += "@" + (*uit)->getNickname() + " ";
        else
            usersList += (*uit)->getNickname() + " ";
    }

    std::string namesMsg = ":" + pollManager.getHostname() + " 353 " + invited->getNickname() + " = " + channelName + " :" + usersList + "\r\n";
    std::string endNamesMsg = ":" + pollManager.getHostname() + " 366 " + invited->getNickname() + " " + channelName + " :End of /NAMES list.\r\n";
    send(invited->getClientFD(), namesMsg.c_str(), namesMsg.size(), 0);
    send(invited->getClientFD(), endNamesMsg.c_str(), endNamesMsg.size(), 0);

    if (!channel.getTopic().empty())
    {
        std::stringstream ss;
        ss << time(NULL);

        std::string topicMsg = ":" + pollManager.getHostname() + " 332 " + invited->getNickname() + " " + channelName + " :" + channel.getTopic() + "\r\n";
        std::string topicSetMsg = ":" + pollManager.getHostname() + " 333 " + invited->getNickname() + " " + channelName + " " + sender.getNickname() + " " + ss.str() + "\r\n";

        send(invited->getClientFD(), topicMsg.c_str(), topicMsg.size(), 0);
        send(invited->getClientFD(), topicSetMsg.c_str(), topicSetMsg.size(), 0);
    }
}
