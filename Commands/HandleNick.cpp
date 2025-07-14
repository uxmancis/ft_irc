#include "Commands.hpp"
#include <sstream>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstdio>

void HandleNICK(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    Client& client = pollManager.getClient(fd);

    if (args.empty() || args[0][0] == '#')
    {
        std::string err = ":irc.local 431 * :Erroneous Nickname\r\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    std::ostringstream oss;
    oss << args[0] << fd;
    std::string requestedNick = oss.str();

    std::string oldNick = client.getNickname();
    client.setNickname(requestedNick);

    std::string nickMsg = ":" + (oldNick.empty() ? requestedNick : oldNick) + "!user@localhost NICK :" + requestedNick + "\r\n";
    send(fd, nickMsg.c_str(), nickMsg.size(), 0);

    std::map<std::string, Channel>& channels = pollManager.getChannels();
    std::map<std::string, Channel>::iterator it;
    
    for (it = channels.begin(); it != channels.end(); ++it)
    {
        Channel& channel = it->second;
        if (channel.hasUser(&client))
        {
            const std::vector<Client*>& users = channel.getRegularUsers();
            std::vector<Client*>::const_iterator userIt;

            for (userIt = users.begin(); userIt != users.end(); ++userIt)
            {
                if ((*userIt)->getClientFD() != fd)
                {
                    send((*userIt)->getClientFD(), nickMsg.c_str(), nickMsg.size(), 0);
                }
            }
        }
    }
}
