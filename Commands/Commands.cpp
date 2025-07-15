#include "Commands.hpp"
#include <sstream>
#include <vector>
#include <ctype.h>
#include <algorithm>


void HandleCommands(int fd, const std::string& command, PollManager& pollManager)
{
    if (command.empty())
        return;

    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    std::vector<std::string> args;
    std::string token;
    bool trailing = false;
    while (iss >> token)
    {
        if (!trailing && token[0] == ':')
        {
            trailing = true;
            std::string rest;
            std::getline(iss, rest);
            token += rest;
            args.push_back(token.substr(1));
            break;
        }
        args.push_back(token);
    }
    std::cout << cmd << std::endl;
    if (cmd == "PASS")
        HandlePASS(fd, args, pollManager);
    else if (cmd == "NICK")
        HandleNICK(fd, args, pollManager);
    else if (cmd == "USER")
        HandleUSER(fd, args, pollManager);
    else if (cmd == "JOIN")
        HandleJOIN(fd, args, pollManager);
    else if (cmd == "PRIVMSG")
        HandleMSG(fd, args, pollManager);
    else if (cmd == "QUIT")
        HandleQUIT(fd, pollManager);
    else if (cmd == "KICK")
        HandleKICK(fd, args, pollManager);
    else if (cmd == "INVITE")
        HandleINVITE(fd, args, pollManager);
    else if (cmd == "TOPIC")
        HandleTOPIC(fd, args, pollManager);
    else if (cmd == "MODE")
        HandleMODE(fd, args, pollManager);
    else if (cmd == "WHO")
        HandleWHO(fd, args, pollManager);
    else if (cmd == "PING")
        HandlePING(fd, args);
    else if (cmd == "PONG")
        HandlePONG(fd, args);
    else if (cmd == "PART")
        HandlePART(fd, args, pollManager);
    else if (cmd == "NAMES")
        HandleNAMES(fd, args, pollManager);
    else if (cmd == "LIST")
        HandleLIST(fd, args, pollManager);
    else if (cmd == "NOTICE")
        HandleNOTICE(fd, args, pollManager);
    else if (cmd == "WHOIS")
        HandleWHOIS(fd, args, pollManager);
    else if (cmd == "MOTD")
        HandleMOTD(fd, pollManager);
    else if (cmd == "ISON")
        HandleISON(fd, args, pollManager);
    else
    {
        std::string err = ":irc.local 421 " + cmd + " :Unknown command\r\n";
        send(fd, err.c_str(), err.size(), 0);
    }
}