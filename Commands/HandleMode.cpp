#include "Commands.hpp"
#include <sstream>
#include <vector>
#include <cstdlib>

void sendModeMessage(Client& client, const std::string& channelName, const std::string& modeStr, PollManager& pollManager)
{
    std::string msg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost MODE " + channelName + " " + modeStr + "\r\n";
    Channel& channel = pollManager.getChannels()[channelName];
    const std::vector<Client*>& users = channel.getRegularUsers();
    std::vector<Client*>::const_iterator it;
    for (it = users.begin(); it != users.end(); ++it)
        send((*it)->getClientFD(), msg.c_str(), msg.size(), 0);
}

void HandleMODEi(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    if (args.size() < 2 || (args[1] != "+i" && args[1] != "-i"))
        return;

    Client& client = pollManager.getClient(fd);
    Channel& channel = pollManager.getChannels()[args[0]];
    channel.setInviteOnly(args[1] == "+i");
    sendModeMessage(client, args[0], args[1], pollManager);
}

void HandleMODEt(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    if (args.size() < 2 || (args[1] != "+t" && args[1] != "-t"))
        return;

    Client& client = pollManager.getClient(fd);
    Channel& channel = pollManager.getChannels()[args[0]];
    channel.setFreeTopic(args[1] == "-t");
    sendModeMessage(client, args[0], args[1], pollManager);
}

void HandleMODEk(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    if ((args[1] == "+k" && args.size() != 3) || (args[1] == "-k" && args.size() != 3))
        return;

    Client& client = pollManager.getClient(fd);
    Channel& channel = pollManager.getChannels()[args[0]];

    if (args[1] == "+k")
    {
        channel.setPrivate(true, args[2]);
        sendModeMessage(client, args[0], "+k", pollManager);
    }
    else if (args[2] == channel.getPassword())
    {
        channel.setPrivate(false, "");
        sendModeMessage(client, args[0], "-k", pollManager);
    }
}

void HandleMODEo(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    std::cout << "target =" << args[1] << "channel_name = " << args[0] << std::endl;
    if (args.size() != 3 || (args[1] != "+o" && args[1] != "-o"))
        return;

    Client& client = pollManager.getClient(fd);
    Channel& channel = pollManager.getChannels()[args[0]];
    std::map<int, Client>& clients = pollManager.getClients();

    Client* target = NULL;
    std::map<int, Client>::iterator it;
    for (it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second.getNickname() == args[2])
        {
            target = &(it->second);
            break;
        }
    }

    if (!target || !channel.hasUser(target))
        return;

    if (args[1] == "+o")
    {
        if (!channel.isAdmin(target))
            channel.addAdmin(target);
    }
    else
    {
        if (channel.isAdmin(target))
            channel.removeAdmin(target);
    }

    sendModeMessage(client, args[0], args[1] + " " + args[2], pollManager);
}

void HandleMODEl(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    if ((args[1] == "+l" && args.size() != 3) || (args[1] == "-l" && args.size() != 2))
        return;

    Client& client = pollManager.getClient(fd);
    Channel& channel = pollManager.getChannels()[args[0]];

    if (args[1] == "+l")
    {
        channel.setLimit(args[2]);
        sendModeMessage(client, args[0], "+l " + args[2], pollManager);
    }
    else
    {
        channel.setLimit("");
        sendModeMessage(client, args[0], "-l", pollManager);
    }
}

void HandleMODE(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    if (args.size() < 2 || args[0].empty() || args[0][0] != '#')
        return;

    Client& client = pollManager.getClient(fd);
    const std::string& channelName = args[0];
    std::map<std::string, Channel>& channels = pollManager.getChannels();

    if (channels.count(channelName) == 0)
        return;

    Channel& channel = channels[channelName];
    if (!channel.isAdmin(&client))
    {
        std::string err = ":irc.local 482 " + channelName + " :You're not a channel operator\r\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    
    const std::string& mode = args[1];
    if (mode == "+i" || mode == "-i")
        HandleMODEi(fd, args, pollManager);
    else if (mode == "+t" || mode == "-t")
        HandleMODEt(fd, args, pollManager);
    else if (mode == "+k" || mode == "-k")
        HandleMODEk(fd, args, pollManager);
    else if (mode == "+o" || mode == "-o")
        HandleMODEo(fd, args, pollManager);
    else if (mode == "+l" || mode == "-l")
        HandleMODEl(fd, args, pollManager);
}
