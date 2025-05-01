#include "Commands.hpp"
#include <sstream>
#include <vector>

void HandleKICK(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    if (args.size() < 1)
    {
        std::string err = "\033[31mUso: /KICK <Usuario>\033[0m\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    Client& sender = pollManager.getClient(fd);
    const std::string& channelName = sender.getActualGroup();
    std::map<std::string, Channel>& channels = pollManager.getChannels();
    if (channels.count(channelName) == 0)
    return;
    Channel& channel = channels[channelName];
    const std::vector<Client*>& admins = channel.getAdmins();
    int auxfd = 0;
    std::string nameNick;
    for (std::vector<Client*>::const_iterator it = admins.begin(); it != admins.end(); ++it)
    {
        
        if ((*it)->getClientFD() == fd)
        {
            std::map<int, Client>::iterator it;
            for (it = pollManager.getClients().begin(); it != pollManager.getClients().end(); ++it)
            {
                Client& client = it->second;
                if (client.getNickname() == args[0])
                {
                    std::string hola = client.getActualGroup();
                    nameNick = client.getNickname();
                    auxfd = client.getClientFD();
                    channels["General"].addUser(&client);
                    client.setActualGroup("General");
                    Client& user = pollManager.getClient(auxfd);
                    std::string msg = GREEN"Echaste a " + user.getNickname() + " del canal " + hola + "\n" RESET;
                    send(fd, msg.c_str(), msg.size(), 0);
                    msg = RED "" + sender.getNickname() + " te ha echado del canal " + hola + "\n" RESET;
                    send(auxfd, msg.c_str(), msg.size(), 0);
                    channel.removeUser(&user);
                    channel.removeAdmin(&user);
                    break;
                }
            }
            if (nameNick == args[0])
                return ; 
            if (pollManager.getClients().count(auxfd) == 0)
            {
                std::string msg = RED "El usuario " + args[0] + " no está en el grupo " + channelName + ".\n" RESET;
                send(fd, msg.c_str(), msg.size(), 0);
                return;
            }
            Client& user = pollManager.getClient(auxfd);
            std::string msg = GREEN"Echaste a " + user.getNickname() + " del canal " + channelName + "\n" RESET;
            send(fd, msg.c_str(), msg.size(), 0);
            msg = RED "" + sender.getNickname() + " te ha echado del canal " + channelName + "\n" RESET;
            send(user.getClientFD(), msg.c_str(), msg.size(), 0);
            channel.removeUser(&user);
            channel.removeAdmin(&user);
            return;
        }
        if (it == admins.end())
        {
            Client& user = pollManager.getClient(auxfd);
            std::string msg = RED "El usuario " + user.getNickname() + " no está en el grupo " + channelName + ".\n" RESET;
            send(fd, msg.c_str(), msg.size(), 0);
        }
    }
    std::string msg =  RED "No eres el admin\n" RESET;
    send(fd, msg.c_str(), msg.size(), 0);
    return;
}
