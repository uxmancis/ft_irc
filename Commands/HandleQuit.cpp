#include "Commands.hpp"
#include <sstream>
#include <vector>

void HandleQUIT(int fd, PollManager& pollManager)
{
    Client& client = pollManager.getClient(fd);
    std::string msg = "Hasta luego, " + client.getNickname() + "\n";
    send(fd, msg.c_str(), msg.size(), 0);
    pollManager.removeClient(&client);
}
