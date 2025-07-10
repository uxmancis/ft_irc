#include "Commands.hpp"
#include <sstream>
#include <vector>

bool isNicknameTaken(const std::string& nick, int currentFd, const std::map<int, Client>& clients)
{
    for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second.getNickname() == nick && it->first != currentFd)
            return true;
    }
    return false;
}

void HandleNICK(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    Client& client = pollManager.getClient(fd);
    if (args.empty() || args[0][0] == '#')
    {
        std::string err = ":irc.local 431 * :Erroneous Nickname\r\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    std::string requestedNick = args[0];
    const std::map<int, Client>& clients = pollManager.getClients();
    if (isNicknameTaken(requestedNick, fd, clients))
        requestedNick += std::to_string(fd);

    std::string oldNick = client.getNickname();
    client.setNickname(requestedNick);
    std::string nickMsg;
    if (oldNick.empty())
        nickMsg = ":" + requestedNick + "!user@localhost NICK :" + requestedNick + "\r\n";
    else
        nickMsg = ":" + oldNick + "!user@localhost NICK :" + requestedNick + "\r\n";
    send(fd, nickMsg.c_str(), nickMsg.size(), 0);
}
