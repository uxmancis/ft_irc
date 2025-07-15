#include "Commands.hpp"
#include <sstream>
#include <vector>
#include <set>

void HandleMOTD(int fd, PollManager& pollManager)
{
    Client& client = pollManager.getClient(fd);

    std::string welcome = ":irc.local 001 " + client.getNickname() + " :Welcome to the IRC server, " + client.getNickname() + "\r\n";
    send(fd, welcome.c_str(), welcome.size(), 0);

    std::string host = ":irc.local 002 " + client.getNickname() + " :Your host is irc.local running version 1.0\r\n";
    send(fd, host.c_str(), host.size(), 0);

    std::string created = ":irc.local 003 " + client.getNickname() + " :This server was created today\r\n";
    send(fd, created.c_str(), created.size(), 0);

    std::string serverInfo = ":irc.local 004 " + client.getNickname() + " irc.local 1.0 o o\r\n";
    send(fd, serverInfo.c_str(), serverInfo.size(), 0);

    std::string startMsg = ":irc.local 375 " + client.getNickname() + " :- irc.local Message of the Day -\r\n";
    send(fd, startMsg.c_str(), startMsg.size(), 0);

    std::string motdLine = ":irc.local 372 " + client.getNickname() + " :- Welcome to irc.local. ¡Have fun!\r\n";
    send(fd, motdLine.c_str(), motdLine.size(), 0);

    std::string endMsg = ":irc.local 376 " + client.getNickname() + " :End of /MOTD command\r\n";
    send(fd, endMsg.c_str(), endMsg.size(), 0);

}

