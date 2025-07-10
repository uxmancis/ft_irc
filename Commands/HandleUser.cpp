#include "Commands.hpp"
#include <sstream>
#include <vector>

void HandleUSER(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    Client& client = pollManager.getClient(fd);

    if (args.size() < 4)
    {
        std::string err = ":irc.local 461 * USER :Not enough parameters\r\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    const std::string& newUser = args[0];
    client.setUsername(newUser);
}
