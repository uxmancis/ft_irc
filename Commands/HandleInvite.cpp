#include "Commands.hpp"
#include <sstream>
#include <vector>

void HandleINVITE(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    if (args.size() < 1)
    {
        std::string err = "Uso: /INVITE <Usuario>\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }

    Client& sender = pollManager.getClient(fd);
    const std::string& channelName = sender.getActualGroup();
    std::map<std::string, Channel>& channels = pollManager.getChannels();

    if (channels.count(channelName) == 0)
        return;

    Channel& channel = channels[channelName];

    // Verificar si el sender es admin
    bool isAdmin = false;
    const std::vector<Client*>& admins = channel.getAdmins();
    for (size_t i = 0; i < admins.size(); ++i)
    {
        if (admins[i]->getClientFD() == fd)
        {
            isAdmin = true;
            break;
        }
    }

    if (!isAdmin)
    {
        std::string err = "No eres administrador del canal.\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }

    // Buscar al usuario a invitar
    Client* invited = NULL;
    std::map<int, Client>& clients = pollManager.getClients();
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second.getNickname() == args[0])
        {
            invited = &(it->second);
            break;
        }
    }

    if (!invited)
    {
        std::string err = "Usuario no encontrado.\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    if (std::atoi(channel.getLimit().c_str()) != 0 && std::atoi(channel.getLimit().c_str()) <= ((int)channel.getRegularUsers().size() + (int)channel.getAdmins().size()))
    {        
        std::string msg = "No te puedes invitar al canal: " + channelName + " esta lleno.\n";
        send(fd, msg.c_str(), msg.size(), 0);
        return ;
    }
    std::string oldGroup = invited->getActualGroup();
    if (channels.count(oldGroup))
        channels[oldGroup].removeUser(invited);
    invited->setActualGroup(channelName);
    channel.addUser(invited);
    std::string msg = "Invitaste a " + invited->getNickname() + " al canal" + channelName + "\n";
    send(fd, msg.c_str(), msg.size(), 0);

    std::string msgToInvited = "Has sido invitado al canal" + channelName + "\n";

    std::string invite_msg = ":" + invited->getNickname() + "!" + sender.getNickname() + "@" + "host" +
                             " INVITE " + invited->getNickname() + " :" + channelName + "\r\n";
    send(invited->getClientFD(), invite_msg.c_str(), invite_msg.length(), 0);


    std::string reply_msg = std::string(":") + "irc_guapitos" + " 341 " + invited->getNickname() +
                            " " + invited->getNickname() + " " + channelName + "\r\n";

    send(sender.getClientFD(), reply_msg.c_str(), reply_msg.length(), 0);
}