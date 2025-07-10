#include "Commands.hpp"
#include <sstream>
#include <vector>
#include <ctype.h>
#include <algorithm>


void HandleCommands(int fd, const std::string& command, PollManager& pollManager)
{
    if (command.empty())
        return;
    std::cout << YELLOW "---- HandleCommands, command: " << command << "----" RESET << std::endl;
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    std::vector<std::string> args;
    std::string arg;
    while (iss >> arg)
        args.push_back(arg);
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
    else if (cmd == "HELP")
        HandleHELP(fd);
    else if (cmd == "ILUSTRATE")
        HandleILUSTRATE(fd, pollManager);
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
    else
    {
        std::string err = ":irc.local 421 " + command + " :Unknown command\r\n";
        send(fd, err.c_str(), err.size(), 0);
    }
}
