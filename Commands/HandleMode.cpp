#include "Commands.hpp"
#include <sstream>
#include <vector>
#include <cstdlib>

void HandleMODEi(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    if (args[0][0] != '+' && args[0][0] != '-')
    {
        std::string error = "MODE no reconocido: " + args[0] + "\n";
        send(fd, error.c_str(), error.size(), 0);
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
            Channel& chan = chIt->second;
            if (args[0][0] == '+')
            {
                chan.setInviteOnly(true);
                std::string msg = "El canal " + chan.getName() + " solo puede ser accesible por invitación.\n";
                send(fd, msg.c_str(), msg.size(), 0);
            }
            if (args[0][0] == '-')
            {
                chan.setInviteOnly(false);
                std::string msg = "El canal " + chan.getName() + " es público (no hace falta invitación).\n";
                send(fd, msg.c_str(), msg.size(), 0);
            }
            return;
        }
    }
}

void HandleMODEt(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    if (args[0][0] != '+' && args[0][0] != '-')
    {
        std::string error = "MODE no reconocido: " + args[0] + "\n";
        send(fd, error.c_str(), error.size(), 0);
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
            Channel& chan = chIt->second;
            if (args[0][0] == '+')
            {
                chan.setFreeTopic(true);
                std::string msg = "El topic del canal " + chan.getName() + " solo puede ser editado por los administradores.\n";
                send(fd, msg.c_str(), msg.size(), 0);
            }
            if (args[0][0] == '-')
            {
                chan.setFreeTopic(false);
                std::string msg = "El topic del canal " + chan.getName() + " puede ser cambiado por cualquier usuario.\n";
                send(fd, msg.c_str(), msg.size(), 0);
            }
            return;
        }
    }
}

void HandleMODEk(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    if ((args[0][0] == '+' && args.size() < 2) || (args[0][0] != '+' && args[0][0] != '-') || (args.size() >= 3)) 
    {
        std::string error = "MODE no reconocido: " + args[0] +  " contraseña\n";
        send(fd, error.c_str(), error.size(), 0);
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
            Channel& chan = chIt->second;
            if (args[0][0] == '+')
            {
                chan.setPrivate(true, args[1]);
                std::string msg = "Ahora el canal " + chan.getName() + " es privado, solo se accede con contraseña.\n";
                send(fd, msg.c_str(), msg.size(), 0);
            }
            if (args[0][0] == '-')
            {
                chan.setPrivate(false, "");
                std::string msg = "Ahora el canal " + chan.getName() + " es público.\n";
                send(fd, msg.c_str(), msg.size(), 0);
            }
            return;
        }
    }
}

void HandleMODEo(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    if (args.size() != 2 || (args[0][0] != '+' && args[0][0] != '-')) 
    {
        std::string error = "MODE no reconocido: " + args[0] +  " usuario\n";
        send(fd, error.c_str(), error.size(), 0);
        return;
    }
    Client& client = pollManager.getClient(fd);
    std::string clannelname = client.getActualGroup();
    Client* invited = NULL;
    std::map<std::string, Channel>& chan = pollManager.getChannels();
    std::vector<Client*> regular = chan[clannelname].getRegularUsers();
    if (args[0][0] == '-')
        regular = chan[clannelname].getAdmins();
    for (size_t i = 0; i < regular.size(); i++)
    {
        if (regular[i]->getNickname() == args[1])
        {
            invited = regular[i];
            break;
        }
    }
    if (!invited)
    {
        std::string err = "Usuario no encontrado.\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    std::map<std::string, Channel>& channels = pollManager.getChannels();
    std::map<std::string, Channel>::iterator chIt;
    for (chIt = channels.begin(); chIt != channels.end(); ++chIt)
    {
        if (chIt->first == clannelname)
        {
            Channel& chan = chIt->second;
            if (args[0][0] == '+')
            {
                chan.addAdmin(invited);
                chan.removeUser(invited);
                std::string msg = "Ahora " + invited->getNickname() + " es administrador.\n";
                send(fd, msg.c_str(), msg.size(), 0);
                msg = client.getNickname() + " te ha añadido como administrador.\n" ;
                send(invited->getClientFD(), msg.c_str(), msg.size(), 0);
            }
            if (args[0][0] == '-')
            {
                chan.removeAdmin(invited);
                chan.addUser(invited);
                chan.setPrivate(false, "");
                std::string msg = invited->getNickname() + " ya no es administrador.\n";
                send(fd, msg.c_str(), msg.size(), 0);
                msg = client.getNickname() + " te ha eliminado como administrador.\n";
                send(invited->getClientFD(), msg.c_str(), msg.size(), 0);
            }
            return;
        }
    }
}

void HandleMODEl(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    if ((args[0][0] == '+' && args.size() < 2) || (args[0][0] == '-' && args.size() < 1)) 
    {
        std::string error = "MODE no reconocido: " + args[0] +  " numero de clientes\n";
        send(fd, error.c_str(), error.size(), 0);
        return;
    }
    if (args[0][0] == '+' && std::atoi(args[1].c_str()) <= 0)
    {
        std::string error = "Numero de clientes no valido\n";
        send(fd, error.c_str(), error.size(), 0);
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
            Channel& chan = chIt->second;
            if (args[0][0] == '+')
            {
                chan.setLimit(args[1]);
                std::string msg = "Ahora el canal " + chan.getName() + " esta limitado a " + args[1] + " clientes\n";
                send(fd, msg.c_str(), msg.size(), 0);
            }
            if (args[0][0] == '-')
            {
                chan.setLimit("");
                std::string msg = "Ahora el canal no tiene limite de clientes\n";
                send(fd, msg.c_str(), msg.size(), 0);
            }
            return;
        }
    }
}

void HandleMODE(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    if (args.size() < 1)
    {
        std::string err = "Uso: /MODE <Opciones>\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    Client& sender = pollManager.getClient(fd);
    const std::string& channelName = sender.getActualGroup();
    std::map<std::string, Channel>& channels = pollManager.getChannels();
    if (channels.count(channelName) == 0)
        return;
    Channel& channel = channels[channelName];
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
    if (args[0] == "+i" || args[0] == "-i")
        HandleMODEi(fd, args, pollManager);
    else if (args[0] == "+t" || args[0] == "-t")
        HandleMODEt(fd, args, pollManager);
    else if (args[0] == "+k" || args[0] == "-k")
        HandleMODEk(fd, args, pollManager);
    else if (args[0] == "+o" || args[0] == "-o")
        HandleMODEo(fd, args, pollManager);
    else if (args[0] == "+l" || args[0] == "-l")
        HandleMODEl(fd, args, pollManager);  
    else
    {
        std::string error = "MODE no reconocido: " + args[0] + "\n";
        send(fd, error.c_str(), error.size(), 0);
    }
    return;
}