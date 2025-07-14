#include "Commands.hpp"
#include <sstream>
#include <vector>


void HandleNAMES(int fd, const std::vector<std::string>& args, PollManager& pollManager) 
{
    Client& client = pollManager.getClient(fd);
    std::string clientNick = client.getNickname();

    std::map<std::string, Channel>& channels = pollManager.getChannels();
    std::string targetChannel = (args.empty() ? "" : args[0]);
    
    if (!targetChannel.empty()) 
    {
        if (channels.find(targetChannel) == channels.end()) 
        {
            std::string err = ":irc.local 366 " + clientNick + " " + targetChannel + " :End of /NAMES list\r\n";
            send(fd, err.c_str(), err.size(), 0);
            return;
        }

        Channel& chan = channels[targetChannel];
        std::string namesLine = "= " + targetChannel + " :";

        const std::vector<Client*>& users = chan.getRegularUsers();
        for (size_t i = 0; i < users.size(); ++i) 
            namesLine += users[i]->getNickname() + " ";

        const std::vector<Client*>& admins = chan.getAdmins();
        for (size_t i = 0; i < admins.size(); ++i)
            namesLine += "@" + admins[i]->getNickname() + " ";

        std::string namesMsg = ":irc.local 353 " + clientNick + " " + namesLine + "\r\n";
        std::string endMsg = ":irc.local 366 " + clientNick + " " + targetChannel + " :End of /NAMES list\r\n";

        send(fd, namesMsg.c_str(), namesMsg.size(), 0);
        send(fd, endMsg.c_str(), endMsg.size(), 0);
    }
    else 
    {
        for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it) 
        {
            const std::string& channelName = it->first;
            Channel& chan = it->second;
            std::string namesLine = "= " + channelName + " :";

            const std::vector<Client*>& users = chan.getRegularUsers();
            for (size_t i = 0; i < users.size(); ++i) 
                namesLine += users[i]->getNickname() + " ";

            const std::vector<Client*>& admins = chan.getAdmins();
            for (size_t i = 0; i < admins.size(); ++i)
                namesLine += "@" + admins[i]->getNickname() + " ";

            std::string namesMsg = ":irc.local 353 " + clientNick + " " + namesLine + "\r\n";
            std::string endMsg = ":irc.local 366 " + clientNick + " " + channelName + " :End of /NAMES list\r\n";

            send(fd, namesMsg.c_str(), namesMsg.size(), 0);
            send(fd, endMsg.c_str(), endMsg.size(), 0);
        }
    }
}
