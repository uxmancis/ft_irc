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

/* void HandleInput(int fd, PollManager& pollManager); */
void HandleInput(int fd, PollManager& pollManager, const std::string& command);
void HandleCommands(int fd, const std::string& command, PollManager& pollManager);
void HandleUSER(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandleNICK(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandleKICK(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandleQUIT(int fd, PollManager& pollManager);
void HandleJOIN(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandleINVITE(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandleTOPIC(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandleMSG(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandleMSG(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandleILUSTRATE(int fd, PollManager& pollManager);
void HandleMODE(int fd, const std::vector<std::string>& args, PollManager& pollManager);
void HandleHELP(int fd);

#endif
