#include "Commands.hpp"
#include <sstream>
#include <vector>
#include <cstdlib>

void HandleISON(int fd, const std::vector<std::string>& args, PollManager& pollManager)
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
    Client& client = pollManager.getClient(fd);
    std::string response = ":irc.local 303 " + client.getNickname() + " :" + result + "\r\n";
    send(fd, response.c_str(), response.size(), 0);
}
