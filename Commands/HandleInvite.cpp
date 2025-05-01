#include "Commands.hpp"
#include <sstream>
#include <vector>

void HandleINVITE(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    if (args.size() < 1)
    {
        std::string err = "\033[31mUso: /INVITE <Usuario>\033[0m\n";
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
        std::string err = "\033[31mNo eres administrador del canal.\033[0m\n";
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
        std::string err = "\033[31mUsuario no encontrado.\033[0m\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    if (std::atoi(channel.getLimit().c_str()) != 0 && std::atoi(channel.getLimit().c_str()) <= ((int)channel.getRegularUsers().size() + (int)channel.getAdmins().size()))
    {        
        std::string msg = "\033[31mNo te puedes invitar al canal: " + channelName + " esta lleno.\033[0m\n";
        send(fd, msg.c_str(), msg.size(), 0);
        return ;
    }
    std::string oldGroup = invited->getActualGroup();
    if (channels.count(oldGroup))
        channels[oldGroup].removeUser(invited);
    invited->setActualGroup(channelName);
    channel.addUser(invited);
    std::string msg = "Invitaste a \033[33m" + invited->getNickname() + "\033[0m al canal \033[36m" + channelName + "\033[0m\n";
    send(fd, msg.c_str(), msg.size(), 0);
    std::string msgToInvited = "Has sido invitado al canal \033[36m" + channelName + "\033[0m\n";
    send(invited->getClientFD(), msgToInvited.c_str(), msgToInvited.size(), 0);
}