#include "Commands.hpp"
#include <sstream>
#include <vector>

void HandleWHOIS(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    if (args.empty()) 
    {
        std::string err = ":irc.local 431 :No nickname given\r\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }

    const std::string& targetNick = args[0];
    Client& requester = pollManager.getClient(fd);

    std::map<int, Client>& allClients = pollManager.getClients();
    for (std::map<int, Client>::iterator it = allClients.begin(); it != allClients.end(); ++it) {
        Client& target = it->second;
        if (target.getNickname() == targetNick) {
            std::string msg1 = ":irc.local 311 " + requester.getNickname() + " " + targetNick + " " +
                target.getUsername() + " localhost * :" + target.getUsername() + "\r\n";
            std::string msg2 = ":irc.local 318 " + requester.getNickname() + " " + targetNick + " :End of /WHOIS list\r\n";
            send(fd, msg1.c_str(), msg1.size(), 0);
            send(fd, msg2.c_str(), msg2.size(), 0);
            return;
        }
    }

    std::string err = ":irc.local 401 " + requester.getNickname() + " " + targetNick + " :No such nick/channel\r\n";
    std::string endMsg = ":irc.local 318 " + requester.getNickname() + " " + targetNick + " :End of /WHOIS list\r\n";
    send(fd, err.c_str(), err.size(), 0);
    send(fd, endMsg.c_str(), endMsg.size(), 0);
}
