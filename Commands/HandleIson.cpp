#include "Commands.hpp"

void Commands::handleISON(int fd, const std::vector<std::string> &args, PollManager &pollManager)
{
    if (args.empty())
        return;
    std::string result;
    for (size_t i = 0; i < args.size(); ++i)
    {
        for (std::map<int, Client>::const_iterator it = pollManager.getClients().begin(); it != pollManager.getClients().end(); ++it)
        {
            if (it->second.getNickname() == args[i])
            {
                if (!result.empty())
                    result += " ";
                result += args[i];
                break;
            }
        }
    }
    Client &client = pollManager.getClient(fd);
    std::string response = ":" + pollManager.getHostname() + " 303 " + client.getNickname() + " :" + result + "\r\n";
    send(fd, response.c_str(), response.size(), 0);
}
