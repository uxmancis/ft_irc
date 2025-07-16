#include "Commands.hpp"
#include "../Poll/PollManager.hpp"
#include <sstream>
#include <vector>
#include <ctype.h>
#include <algorithm>
#include <signal.h>

// Orthodox Canonical Form Implementation
Commands::Commands() {}

Commands::Commands(const Commands &other) { (void)other; }

Commands &Commands::operator=(const Commands &other)
{
    (void)other;
    return *this;
}
Commands::~Commands() {}

// Helper methods implementation
std::vector<std::string> Commands::parseArguments(const std::string &command)
{
    std::vector<std::string> args;
    std::istringstream iss(command);
    std::string token;
    bool trailing = false;
    iss >> token; // skip command itself
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
    return args;
}

std::string Commands::toUpperCase(const std::string &str)
{
    std::string result(str);
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

void Commands::handleCommand(int fd, const std::string &command, PollManager &pollManager)
{
    if (command.empty())
        return;
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    cmd = toUpperCase(cmd);
    std::vector<std::string> args = parseArguments(command);
    // std::cout << "---------- Command: " << cmd << args[0] << std::endl;
    if (cmd == "PASS")
        handlePASS(fd, args, pollManager);
    else if (cmd == "NICK")
        handleNICK(fd, args, pollManager);
    else if (cmd == "USER")
        handleUSER(fd, args, pollManager);
    else if (cmd == "JOIN")
        handleJOIN(fd, args, pollManager);
    else if (cmd == "PRIVMSG")
        handleMSG(fd, args, pollManager);
    else if (cmd == "QUIT")
        handleQUIT(fd, pollManager);
    else if (cmd == "KICK")
        handleKICK(fd, args, pollManager);
    else if (cmd == "INVITE")
        handleINVITE(fd, args, pollManager);
    else if (cmd == "TOPIC")
        handleTOPIC(fd, args, pollManager);
    else if (cmd == "MODE")
        handleMODE(fd, args, pollManager);
    else if (cmd == "WHO")
        handleWHO(fd, args, pollManager);
    else if (cmd == "PING")
        handlePING(fd, args, pollManager);
    else if (cmd == "PONG")
        handlePONG(fd, pollManager);
    else if (cmd == "PART")
        handlePART(fd, args, pollManager);
    else if (cmd == "NAMES")
        handleNAMES(fd, args, pollManager);
    else if (cmd == "LIST")
        handleLIST(fd, args, pollManager);
    else if (cmd == "NOTICE")
        handleNOTICE(fd, args, pollManager);
    else if (cmd == "WHOIS")
        handleWHOIS(fd, args, pollManager);
    else if (cmd == "MOTD")
        handleMOTD(fd, pollManager);
    else if (cmd == "ISON")
        handleISON(fd, args, pollManager);
    else
    {
        std::string err = ":" + pollManager.getHostname() + " 421 " + cmd + " :Unknown command\r\n";
        send(fd, err.c_str(), err.size(), 0);
    }
}