#include "Commands.hpp"
#include <sstream>
#include <vector>

void HandleNOTICE(int fd, const std::vector<std::string>& args, PollManager& pollManager) 
{
    if (args.size() < 2)
        return; 

    Client& sender = pollManager.getClient(fd);
    std::string target = args[0];

    std::string message;
    for (size_t i = 1; i < args.size(); ++i) 
    {
        message += args[i];
        if (i != args.size() - 1)
            message += " ";
    }
    std::string fullMsg = ":" + sender.getNickname() + "!" + sender.getUsername() + "@localhost NOTICE " + target + " :" + message + "\r\n";

    if (target[0] == '#') 
    {
        std::map<std::string, Channel>& channels = pollManager.getChannels();
        if (channels.count(target)) {
            Channel& chan = channels[target];
            const std::vector<Client*>& users = chan.getRegularUsers();
            for (size_t i = 0; i < users.size(); ++i) {
                if (users[i]->getClientFD() != fd)
                    send(users[i]->getClientFD(), fullMsg.c_str(), fullMsg.size(), 0);
            }
        }
    }
    else
    {
        std::map<int, Client>& clients = pollManager.getClients();
        for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) 
        {
            if (it->second.getNickname() == target) 
            {
                send(it->first, fullMsg.c_str(), fullMsg.size(), 0);
                break;
            }
        }
    }
}
