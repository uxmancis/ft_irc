#include "Commands.hpp"
#include <sstream>
#include <vector>

void HandleUSER(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    if (args.size() < 1)
    {
        std::string err = "\033[31mUso: /USER <nuevo_Username>\033[0m\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }

    Client& client = pollManager.getClient(fd);
    const std::string& newNick = args[0];
    if (newNick == client.getUsername())
    {
        std::string err = "\033[31mUsaste el mismo Username\033[0m\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    const std::map<int, Client>& clients = pollManager.getClients();
    for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second.getUsername() == newNick)
        {
            std::string err = "\033[31mEse Username ya esta en uso\033[0m\n";
            send(fd, err.c_str(), err.size(), 0);
            return;
        }
    }
    client.setUsername(newNick);
    std::string msg = "\033[32mTu Username ha sido actualizado a: " + newNick + "\033[0m\n";
    send(fd, msg.c_str(), msg.size(), 0);
}
