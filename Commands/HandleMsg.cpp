#include "Commands.hpp"
#include <sstream>
#include <vector>

void HandleMSG(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    if (args.size() < 2)
    {
        std::string err = "\033[31mUso: /MSG <Usuario> <Mensaje>\033[0m\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    Client &aux = pollManager.getClient(fd);
    std::string str = "[" + aux.getNickname() + "] " ;
    std:: string msg;
    std::map<int, Client>::iterator it;
    for (it = pollManager.getClients().begin(); it != pollManager.getClients().end(); ++it)
    {
        const Client& client = it->second;
        if (client.getNickname() == args[0])
        {
            send(client.getClientFD(), str.c_str(), str.size(), 0);
            for (size_t i = 1; i < args.size(); i++)
            {
                send(client.getClientFD(), args[i].c_str(), args[i].size(), 0);
                send(client.getClientFD(), " ", 1, 0);
                msg += args[i] + " ";
            }
            send(client.getClientFD(), "\n", 2, 0);
            break ;
        }
    }
    if (it == pollManager.getClients().end())
        send(fd, "El Usuario no existe\n", 22, 0);
    else
    {
        std::string clear_line = "\033[F\033[2K\033[F\033[2K";
        std::string prefix = "[yo]-> " + aux.getNickname() + " ";
        std::string full_message = prefix + msg + "\n";
        send(fd, clear_line.c_str(), clear_line.size(), 0);
        send(fd, full_message.c_str(), full_message.size(), 0);
        send(fd, "\r", 1, 0);
    }
}
