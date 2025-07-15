#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "../Poll/PollManager.hpp"
#include "../Client/Client.hpp"
#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <map>

class PollManager;

void HandleCommands(int fd, const std::string& command, PollManager& pollManager);
void HandlePASS(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandleUSER(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandleNICK(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandleKICK(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandleQUIT(int fd, PollManager& pollManager);
void HandleJOIN(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandleINVITE(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandleTOPIC(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandleMSG(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandleMODE(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandleWHO(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandlePING(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandlePONG(int fd, PollManager& pollManager);
void HandlePART(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandleNAMES(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandleLIST(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandleNOTICE(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandleWHOIS(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandleMOTD(int fd, PollManager& pollManager);
void HandleISON(int fd, const std::vector<std::string>& args, PollManager& pollManager);


#endif
