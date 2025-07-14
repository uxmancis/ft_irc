#include "Commands.hpp"
#include <sstream>
#include <vector>
#include <set>

void HandleLIST(int fd, const std::vector<std::string>& args, PollManager& pollManager) 
{
    Client& client = pollManager.getClient(fd);
    std::string nick = client.getNickname();
    std::map<std::string, Channel>& channels = pollManager.getChannels();

    std::string header = ":irc.local 321 " + nick + " Channel :Users Name\r\n";
    send(fd, header.c_str(), header.size(), 0);

    std::set<std::string> targetChannels;
    if (!args.empty()) 
    {
        std::stringstream ss(args[0]);
        std::string chan;
        while (std::getline(ss, chan, ','))
            targetChannels.insert(chan);
    }

    std::map<std::string, Channel>::iterator it;
    for (it = channels.begin(); it != channels.end(); ++it) 
    {
        const std::string& chanName = it->first;
        Channel& chan = it->second;
        if (!targetChannels.empty() && targetChannels.find(chanName) == targetChannels.end())
            continue;

        if (chan.isPrivate() && !chan.hasUser(&client))
            continue;

        int userCount = (int)(chan.getRegularUsers().size());

        std::ostringstream oss;
        oss << userCount;
        std::string userCountStr = oss.str();

        std::string topic = chan.getTopic();
        if (topic.empty())
            topic = "";

        std::string line = ":irc.local 322 " + nick + " " + chanName + " " + userCountStr + " :" + topic + "\r\n";
        send(fd, line.c_str(), line.size(), 0);
    }

    std::string footer = ":irc.local 323 " + nick + " :End of /LIST\r\n";
    send(fd, footer.c_str(), footer.size(), 0);
}
