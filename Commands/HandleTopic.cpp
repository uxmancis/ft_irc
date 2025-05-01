#include "Commands.hpp"
#include <sstream>
#include <vector>
#include <algorithm> 

void HandleTOPIC(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    if (args.size() < 1)
    {
        std::string err = "\033[31mUso: /TOPIC <channel topic>\033[0m\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    Client& client = pollManager.getClient(fd);
    std::string clannelname = client.getActualGroup();
    std::map<std::string, Channel>& channels = pollManager.getChannels();
    std::map<std::string, Channel>::iterator chIt;
    for (chIt = channels.begin(); chIt != channels.end(); ++chIt)
    {
        if (chIt->first == clannelname)
        {
            std::cout << "ESTOY AQUI" << std::endl;
            Channel& chan = chIt->second;
            std::string topic;
            bool isAdmin = false;
            std::vector<Client*> admins = chan.getAdmins();
            for (size_t i = 0; i < admins.size(); ++i)
            {
                if (admins[i]->getClientFD() == client.getClientFD())
                {
                    isAdmin = true;
                    break;
                }
            }
            if (chan.isFreeTopic() &&  !isAdmin)
            {
                std::string msg = "\033[32mEl topic del canal: " + clannelname + " no puede ser cambiado por faltas de permisos (no eres administrador).\033[0m\n";
                send(fd, msg.c_str(), msg.size(), 0);
                return;
            }
            for (size_t i = 0; i < args.size(); i++)
            {
                topic += args[i] + " ";
            }
            std::string msg = "\033[32mEl topic del canal: " + clannelname + " se ha cambiado de " + chan.getTopic() + "a " + topic + "\033[0m\n";
            send(fd, msg.c_str(), msg.size(), 0);
            chan.setChannelTopic(topic);
            return;
        }
    }
}
