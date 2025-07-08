#include "Commands.hpp"
#include <sstream>
#include <vector>
#include <cstdlib>

void HandleJOIN(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    if (args.size() < 1)
    {
        std::string err = "\033[31mUso: /JOIN <canal>\033[0m\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    std::string channelName = args[0];
    Client& client = pollManager.getClient(fd);
    if (pollManager.getChannels().count(channelName) > 0)
    {
        Channel& channel = pollManager.getChannels()[channelName];
        for (size_t i = 0; i < channel.getRegularUsers().size(); ++i)
        {
            if (channel.getRegularUsers()[i] == &client)
            {
                std::string msg = "\033[33mYa estás en el canal: " + channelName + "\033[0m\n";
                send(fd, msg.c_str(), msg.size(), 0);
                return;
            }
        }
        if (std::atoi(channel.getLimit().c_str()) != 0 && std::atoi(channel.getLimit().c_str()) <= ((int)channel.getRegularUsers().size() + (int)channel.getAdmins().size()))
        {        
            std::string msg = "\033[31mNo te puedes unir al canal: " + channelName + " esta lleno.\033[0m\n";
            send(fd, msg.c_str(), msg.size(), 0);
            return ;
        }
        if (channel.isInviteOnly())
        {        
            std::string msg = "\033[31mNo te puedes unir al canal: " + channelName + " es solo por invitacion.\033[0m\n";
            send(fd, msg.c_str(), msg.size(), 0);
            return ;
        }
        if (channel.isPrivate())
        {
            if (args.size() < 2)
            {
                std::string msg = "\033[31mEste canal requiere contraseña. Uso: /JOIN <canal> <contraseña>.\033[0m\n";
                send(fd, msg.c_str(), msg.size(), 0);
                return;
            }
            if (args[1] != channel.getPassword())
            {
                std::string msg = "\033[31mContraseña incorrecta.\033[0m\n";
                send(fd, msg.c_str(), msg.size(), 0);
                return;
            }
        }
        channel.addUser(&client);
        if (!client.getActualGroup().empty())
        {
            Channel& oldChannel = pollManager.getChannels()[client.getActualGroup()];
            oldChannel.removeUser(&client);
            oldChannel.removeAdmin(&client);
        }
        client.setActualGroup(channelName);
        std::string msg = "\033[32mTe has unido al canal: " + channelName + "\033[0m\n";
        send(fd, msg.c_str(), msg.size(), 0);
    }
    else
    {
        if (!client.getActualGroup().empty())
        {
            Channel& oldChannel = pollManager.getChannels()[client.getActualGroup()];
            oldChannel.removeUser(&client);
        }
        Channel newChannel(channelName, "", false, false);
        pollManager.getChannels()[channelName] = newChannel;
        Channel& newChannelRef = pollManager.getChannels()[channelName];
        newChannelRef.addAdmin(&client);
        client.setActualGroup(channelName);
        std::string msg = "\033[32mHas creado y te has unido al canal: " + channelName + ", ahora eres administrador de este grupo.\033[0m\n";
        send(fd, msg.c_str(), msg.size(), 0);
        std::string help =
        "\033[32m[Comandos disponibles siendo administrador del grupo]\033[0m\n"
        "/KICK             - Echar a alguién del grupo actual\n"
        "/INVITE <canal>   - Meter a alguien en el grupo\n"
        "/TOPIC <nombre>   - Cambair el topic del grupo (por defecto no hay topic)\n"
        "/MODE - Change the channels mode:\n"
        "        · i: Set/remove Invite-only channel\n"
        "        · t: Set/remove the restrictions of the TOPIC command to channel operators\n"
        "        · k: Set/remove the channel key (password)\n"
        "        · o: Give/take channel operator privilege\n"
        "        · l: Set/remove the user limit to channel\n"
        "\n";
        send(fd, help.c_str(), help.size(), 0);

        /* sendJoinReplies() */
       std::string response;
       response += ":" + client.getNickname() + "!" + client.getUsername() + "@" + "host" + " JOIN :" + channelName + "\r\n";
           /* Mensajes adicionales opcionales para que figuren en la nueva pestaña */
           response += ":irc_guapitos 332 " + client.getNickname() + " " + channelName + " :" + "topic" + "\r\n";
           response += ":irc_guapitos 333 " + client.getNickname() + " " + channelName + " " + "setby" + " " + "std::to_string(timestamp)" + "\r\n";
           response += ":irc_guapitos 353 " + client.getNickname() + " = " + channelName + " :@" + client.getNickname() + "\r\n";
           response += ":irc_guapitos 366 " + client.getNickname() + " " + channelName + " :End of /NAMES list.\r\n";
       send(fd, response.c_str(), response.size(), 0);
    }
}
