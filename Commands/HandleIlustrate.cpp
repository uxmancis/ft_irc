#include "Commands.hpp"
#include <sstream>
#include <vector>

void HandleILUSTRATE(int fd, PollManager& pollManager)
{
    std::ostringstream oss;
    oss << "\033[35m[Usuarios conectados]\033[0m\n";
    std::map<int, Client>::iterator it;
    for (it = pollManager.getClients().begin(); it != pollManager.getClients().end(); ++it)
    {
        const Client& client = it->second;
        if (!client.getNickname().empty())
            oss << "- " << client.getNickname() << "\n";
        else
            oss << "- FD " << client.getClientFD() << "\n";
    }
    oss << "\033[36m[Grupos (canales)]\033[0m\n";
    std::map<std::string, Channel>& channels = pollManager.getChannels();
    std::map<std::string, Channel>::iterator chIt;
    for (chIt = channels.begin(); chIt != channels.end(); ++chIt)
    {
        const Channel& chan = chIt->second;
        int total = chan.getAdmins().size() + chan.getRegularUsers().size();
        oss << "- " << chIt->first << " (" << total << " usuarios conectados)\n";
    }
    std::string result = oss.str();
    send(fd, result.c_str(), result.size(), 0);
}
