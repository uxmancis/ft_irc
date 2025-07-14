#include "Commands.hpp"
#include <sstream>
#include <vector>
#include <set>

void HandleMOTD(int fd, PollManager& pollManager)
{
    Client& client = pollManager.getClient(fd);
    std::string nick = client.getNickname();

    std::string startMsg = ":irc.local 375 " + nick + " :- irc.local Message of the Day -\r\n";
    std::string motdLine = ":irc.local 372 " + nick + " :- Bienvenido a irc.local. ¡Pásalo bien!\r\n";
    std::string endMsg = ":irc.local 376 " + nick + " :End of /MOTD command\r\n";

    send(fd, startMsg.c_str(), startMsg.size(), 0);
    send(fd, motdLine.c_str(), motdLine.size(), 0);
    send(fd, endMsg.c_str(), endMsg.size(), 0);
}
