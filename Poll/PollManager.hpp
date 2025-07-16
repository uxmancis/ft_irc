#ifndef POLLMANAGER_HPP
#define POLLMANAGER_HPP

#include <vector>
#include <string>
#include <map>
#include <poll.h>
#include <cstdlib>
#include "../Client/Client.hpp"
#include "../Colors.hpp"
#include "../Channel/Channel.hpp"

// Forward declaration
class Commands;

#define BUFFER_SIZE 512
#define PING_TIMEOUT 10

class PollManager
{
    public:
        PollManager(int serverFD, const std::string &password, const std::string &host);
        PollManager(const PollManager &other);
        PollManager &operator=(const PollManager &other);
        ~PollManager();

        void                                run();
        void                                acceptNewClient();
        void                                removeClient(Client *client);

        const std::string                   &getPassword() const;
        std::vector<pollfd>                 &getFDs();
        std::map<std::string, Channel>      &getChannels();
        std::map<int, Client>               &getClients();
        Client                              &getClient(int fd);
        const std::string                   &getHostname() const;
        int                                 getServerFD() const;

    private:
        int                                 _serverFD;
        std::string                         _password;
        std::string                         _hostname;
        std::vector<pollfd>                 _fds;
        std::map<int, Client>               _clients;
        std::map<std::string, Channel>      _channels;
};

#endif
