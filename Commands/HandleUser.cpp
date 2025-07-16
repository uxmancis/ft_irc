#include "Commands.hpp"
#include <sstream>
#include <vector>

void Commands::handleUSER(int fd, const std::vector<std::string> &args, PollManager &pollManager)
{
    Client &client = pollManager.getClient(fd);

    if (args.size() < 4)
    {
        std::string err = ":" + pollManager.getHostname() + " 461 * USER :Not enough parameters\r\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    client.setUsername(args[0]);
}
