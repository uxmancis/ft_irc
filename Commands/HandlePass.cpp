#include "Commands.hpp"
#include <sstream>
#include <vector>

void Commands::handlePASS(int fd, const std::vector<std::string> &args, PollManager &pollManager)
{
    Client &client = pollManager.getClient(fd);
    if (client.getState() == AWAITING_PASSWORD)
    {
        if (args.empty())
        {
            std::string err = ":" + pollManager.getHostname() + " 461 * PASS :Not enough parameters\r\n";
            send(fd, err.c_str(), err.size(), 0);
            pollManager.removeClient(&client);
            return;
        }
        const std::string &providedPass = args[0];
        if (providedPass != pollManager.getPassword())
        {
            std::string err = ":" + pollManager.getHostname() + " 464 * :Password incorrect\r\n";
            send(fd, err.c_str(), err.size(), 0);
            pollManager.removeClient(&client);
            return;
        }
        client.setPassword(providedPass);
    }
}