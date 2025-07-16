#include "Commands.hpp"

void Commands::handleMOTD(int fd, PollManager &pollManager)
{
    Client &client = pollManager.getClient(fd);
    std::string hostname = pollManager.getHostname();

    std::string welcome = ":" + hostname + " 001 " + client.getNickname() + " :Welcome to the IRC server, " + client.getNickname() + "\r\n";
    send(fd, welcome.c_str(), welcome.size(), 0);

    std::string host = ":" + hostname + " 002 " + client.getNickname() + " :Your host is " + hostname + " running version 1.0\r\n";
    send(fd, host.c_str(), host.size(), 0);

    std::string created = ":" + hostname + " 003 " + client.getNickname() + " :This server was created today\r\n";
    send(fd, created.c_str(), created.size(), 0);

    std::string serverInfo = ":" + hostname + " 004 " + client.getNickname() + " " + hostname + " 1.0 o o\r\n";
    send(fd, serverInfo.c_str(), serverInfo.size(), 0);

    std::string startMsg = ":" + hostname + " 375 " + client.getNickname() + " :- " + hostname + " Message of the Day -\r\n";
    send(fd, startMsg.c_str(), startMsg.size(), 0);

    std::string motdLine = ":" + hostname + " 372 " + client.getNickname() + " :- Welcome to " + hostname + ". ¡Have fun!\r\n";
    send(fd, motdLine.c_str(), motdLine.size(), 0);

    std::string endMsg = ":" + hostname + " 376 " + client.getNickname() + " :End of /MOTD command\r\n";
    send(fd, endMsg.c_str(), endMsg.size(), 0);
}
