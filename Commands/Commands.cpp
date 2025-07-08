#include "Commands.hpp"
#include <sstream>
#include <vector>

void HandleCommands(int fd, const std::string& command, PollManager& pollManager)
{
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    std::vector<std::string> args;
    std::string arg;
    while (iss >> arg) 
        args.push_back(arg);
    if (cmd == "JOIN" || cmd == "join")
        HandleJOIN(fd, args, pollManager);
    else if (cmd == "NICK" || cmd == "nick")
        HandleNICK(fd, args, pollManager);
    else if (cmd == "USER" || cmd == "user")
        HandleUSER(fd, args, pollManager);
    else if (cmd == "MSG" || cmd == "msg")
        HandleMSG(fd, args, pollManager);
    else if (cmd == "HELP" || cmd == "help")
        HandleHELP(fd);
    else if (cmd == "ILUSTRATE" || cmd == "ilustrate")
        HandleILUSTRATE(fd, pollManager);
    else if (cmd == "QUIT" || cmd == "quit")
        HandleQUIT(fd, pollManager);
    else if (cmd == "KICK" || cmd == "kick")
        HandleKICK(fd, args, pollManager);
    else if (cmd == "INVITE" || cmd == "invite")
        HandleINVITE(fd, args, pollManager);
    else if (cmd == "TOPIC" || cmd == "topic")
        HandleTOPIC(fd, args, pollManager);
    else if (cmd == "MODE" || cmd == "mode")
        HandleMODE(fd, args, pollManager);   
/*     else
    {
        std::string error = "\033[31mComando no reconocido: " + cmd + "\033[0m\n";
        send(fd, error.c_str(), error.size(), 0);
    } */
}


void HandleMsg(int fd, const std::string& msg, PollManager& pollManager)
{
    Client& sender = pollManager.getClient(fd);
    const std::string& channelName = sender.getActualGroup();
    std::map<std::string, Channel>& channels = pollManager.getChannels();
    if (channels.count(channelName) == 0)
    return;
    Channel& channel = channels[channelName];
    std::string fullMsg = "[" + channelName + "] \033[36m<" + sender.getNickname() + ">\033[0m " + msg + "\n";
    // Enviar a admins
    const std::vector<Client*>& admins = channel.getAdmins();
    for (std::vector<Client*>::const_iterator it = admins.begin(); it != admins.end(); ++it)
    {
        if ((*it)->getClientFD() != fd)
            send((*it)->getClientFD(), fullMsg.c_str(), fullMsg.length(), 0);
    }
    // Enviar a usuarios normales
    const std::vector<Client*>& users = channel.getRegularUsers();
    for (std::vector<Client*>::const_iterator it = users.begin(); it != users.end(); ++it)
    {
        if ((*it)->getClientFD() != fd)
            send((*it)->getClientFD(), fullMsg.c_str(), fullMsg.length(), 0);
    }
    std::string clear_line = "\033[F\033[2K";
    std::string prefix = "[" + channelName + "] \033[36m<yo>\033[0m ";
    std::string full_message = prefix + msg + "\n";
    send(fd, clear_line.c_str(), clear_line.size(), 0);
    send(fd, full_message.c_str(), full_message.size(), 0);
    send(fd, "\r", 1, 0);
}

/* void HandleInput(int fd, PollManager& pollManager)
{
    char buffer[512];
    int bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
    if (!buffer[0])
    {
        std::string clear_line = "\033[F\033[2K"; 
        send(fd, clear_line.c_str(), clear_line.size(), 0);
        return ;
    }
    buffer[bytes] = '\0';
    std::string command(buffer);
    if (!command.empty() && command[command.length() - 1] == '\n')
    command.erase(command.length() - 1);
    if (!command.empty() && command[command.length() - 1] == '\r')
    command.erase(command.length() - 1);
   char c = command[0];
     if (c == '/')
        HandleCommands(fd, command, pollManager);
    else
        HandleMsg(fd, command, pollManager);
    return ;
} */

void HandleInput(int fd, PollManager& pollManager, const std::string& command)
{
    if (command.empty())
        return;

    HandleCommands(fd, command, pollManager);

    HandleMsg(fd, command, pollManager);

}

