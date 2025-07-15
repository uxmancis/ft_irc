#ifndef CHANNEL_HPP
#define CHANNEL_HPP

# include "../Client/Client.hpp"
# include <string>
# include <vector>
# include <cstdlib>

class Client;

class Channel
{
    public:
        Channel();
        Channel(std::string channelName, std::string pass, bool priv, bool invite);
        Channel(const Channel& other);
        Channel& operator=(const Channel& other);
        ~Channel();

        void addAdmin(Client* client);
        void addUser(Client* client);
        void removeUser(Client* client);
        void removeAdmin(Client* client);

        const std::string& getName() const;
        const std::string& getTopic() const;
        const std::string& getPassword() const;
        const std::string& getLimit() const;
        const std::vector<Client*>& getAdmins() const;
        const std::vector<Client*>& getRegularUsers() const;
        
        void setChannelTopic(const std::string& topic);
        void setInviteOnly(bool invite);   
        void setPrivate(bool priv, std::string pass);
        void setFreeTopic(bool priv);
        void setLimit(const std::string limit);
        
        bool isPrivate() const;
        bool isInviteOnly() const;
        bool isFreeTopic() const;
        bool hasUser(Client* client) const;
        bool isFull() const;
        bool isAdmin(Client* client) const;

    private:
        std::string             _name;
        std::string             _pass;
        bool                    _private;
        bool                    _freetopic;
        bool                    _invite;
        std::vector<Client*>    _adminUsers;
        std::vector<Client*>    _regularUsers;
        std::string             _topic;
        std::string             _limit;
};

#endif
