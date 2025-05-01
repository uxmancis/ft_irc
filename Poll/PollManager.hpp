#ifndef POLLMANAGER_HPP
# define POLLMANAGER_HPP

# include <vector>
# include <string>
# include <map>
# include <poll.h>
# include <cstdlib>
# include "../Client/Client.hpp"
# include "../Colors.hpp"
# include "../Channel/Channel.hpp"
# include "../Commands/Commands.hpp"

class PollManager 
{
    public:
        PollManager(int serverFD, const std::string& password);
        ~PollManager();
        PollManager(const PollManager& other);
        PollManager& operator=(const PollManager& other);

        void                    run();
        void                    acceptNewClient();

        const std::string&              getPassword() const;
        std::vector<pollfd>&            getFDs();
        int                             getServerFD() const;
        std::map<std::string, Channel>& getChannels();
        Client&                         getClient(int fd);
        std::map<int, Client>&          getClients();

        std::string                     requestInput(int clientFD, const std::string& prompt);
        void                            removeClient(Client* client);
    private:
        std::string                                 _password;
        int                                         _serverFD;
        std::vector<pollfd>                         _fds;
        std::map<int, Client>                       _clients;
        std::map<std::string, Channel>              _channels;
};

#endif
