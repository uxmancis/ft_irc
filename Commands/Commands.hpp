#ifndef COMMANDS_HPP
#define COMMANDS_HPP


#include "../Client/Client.hpp"
#include "../Poll/PollManager.hpp"
#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <map>
#include <vector>
#include <ctime>
#include <cstdlib>

// Forward declaration
class PollManager;

class Commands
{
    public:
        Commands();
        Commands(const Commands &other);
        Commands &operator=(const Commands &other);
        ~Commands();

        // Main command handler
        static void handleCommand(int fd, const std::string &command, PollManager &pollManager);

        static void handleMOTD(int fd, PollManager &pollManager);
        static void handlePING(int fd, const std::vector<std::string> &args, PollManager &pollManager);
        static void handleQUIT(int fd, PollManager &pollManager);
        static void handleJOIN(int fd, const std::vector<std::string> &args, PollManager &pollManager);


    private:
        // Command handlers
        static void handlePASS(int fd, const std::vector<std::string> &args, PollManager &pollManager);
        static void handleUSER(int fd, const std::vector<std::string> &args, PollManager &pollManager);
        static void handleNICK(int fd, const std::vector<std::string> &args, PollManager &pollManager);
        static void handleKICK(int fd, const std::vector<std::string> &args, PollManager &pollManager);
        static void handleINVITE(int fd, const std::vector<std::string> &args, PollManager &pollManager);
        static void handleTOPIC(int fd, const std::vector<std::string> &args, PollManager &pollManager);
        static void handleMSG(int fd, const std::vector<std::string> &args, PollManager &pollManager);
        static void handleMODE(int fd, const std::vector<std::string> &args, PollManager &pollManager);
        static void handleWHO(int fd, const std::vector<std::string> &args, PollManager &pollManager);
        static void handlePONG(int fd, PollManager &pollManager);
        static void handlePART(int fd, const std::vector<std::string> &args, PollManager &pollManager);
        static void handleNAMES(int fd, const std::vector<std::string> &args, PollManager &pollManager);
        static void handleLIST(int fd, const std::vector<std::string> &args, PollManager &pollManager);
        static void handleNOTICE(int fd, const std::vector<std::string> &args, PollManager &pollManager);
        static void handleWHOIS(int fd, const std::vector<std::string> &args, PollManager &pollManager);
        static void handleISON(int fd, const std::vector<std::string> &args, PollManager &pollManager);

        // Helper methods
        static std::vector<std::string> parseArguments(const std::string &command);
        static std::string toUpperCase(const std::string &str);
};

#endif
