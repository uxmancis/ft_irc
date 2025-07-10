#include "Commands.hpp"
#include <sstream>
#include <vector>
#include <cstdlib>


void HandleJOIN(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    if (args.empty())
    {
        std::string err = ":irc.local 461 * JOIN :Not enough parameters\r\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }

    std::string channelName = args[0];
    if (channelName[0] != '#')
        channelName = "#" + channelName;

    Client& client = pollManager.getClient(fd);
    std::map<std::string, Channel>& channels = pollManager.getChannels();

    Channel* channelPtr = nullptr;

    // Si no existe el canal, crear uno nuevo
    if (channels.count(channelName) == 0)
    {
        channels[channelName] = Channel(channelName, "", false, false);
        channels[channelName].addAdmin(&client);
    }
    channelPtr = &channels[channelName];

    Channel& channel = *channelPtr;

    // Si ya está en otro canal, quitarlo
    if (!client.getActualGroup().empty() && client.getActualGroup() != channelName)
    {
        Channel& oldChannel = channels[client.getActualGroup()];
        oldChannel.removeUser(&client);
        oldChannel.removeAdmin(&client);
    }

    // Añadir al usuario al canal (si no está)
    if (!channel.hasUser(&client))
    {
        channel.addUser(&client);
        client.setActualGroup(channelName);
    }

    // Construir mensaje JOIN
    std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost JOIN " + channelName + "\r\n";

    // Enviar JOIN a todos los usuarios del canal
    for (Client* user : channel.getRegularUsers())
    {
        send(user->getClientFD(), joinMsg.c_str(), joinMsg.size(), 0);
    }

    // Enviar lista de usuarios (353) al cliente que entra
    std::string namesList = ":irc.local 353 " + client.getNickname() + " = " + channelName + " :";
    const std::vector<Client*>& users = channel.getRegularUsers();
    for (size_t i = 0; i < users.size(); ++i)
    {
        namesList += users[i]->getNickname();
        if (i != users.size() - 1)
            namesList += " ";
    }
    namesList += "\r\n";
    send(fd, namesList.c_str(), namesList.size(), 0);

    // Enviar fin de lista (366)
    std::string endOfNames = ":irc.local 366 " + client.getNickname() + " " + channelName + " :End of /NAMES list.\r\n";
    send(fd, endOfNames.c_str(), endOfNames.size(), 0);
}

