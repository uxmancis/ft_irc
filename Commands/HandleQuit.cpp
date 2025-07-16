#include "Commands.hpp"

void Commands::handleQUIT(int fd, PollManager &pollManager)
{
    Client &client = pollManager.getClient(fd);
    std::string quitMsg = ":" + client.getNickname() + "!user@" + pollManager.getHostname() + " QUIT :Bye!\r\n";

    std::map<std::string, Channel> &channels = pollManager.getChannels();
    for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        Channel &channel = it->second;
        if (channel.hasUser(&client))
        {
            std::vector<Client *> users = channel.getRegularUsers();
            for (size_t i = 0; i < users.size(); ++i)
            {
                if (users[i]->getClientFD() != fd)
                    send(users[i]->getClientFD(), quitMsg.c_str(), quitMsg.size(), 0);
            }
            channel.removeUser(&client);
        }
    }

    send(fd, quitMsg.c_str(), quitMsg.size(), 0);
    pollManager.removeClient(&client);
}
