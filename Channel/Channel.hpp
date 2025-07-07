#ifndef CHANNEL_HPP
#define CHANNEL_HPP

# include "../Client/Client.hpp"
# include <string>
# include <vector>

class Client;

class Channel
{
    public:
        Channel(); /*Default constructor*/
        Channel(std::string channelName, std::string pass, bool priv, bool invite); /*Parameterized constructor*/
        Channel(const Channel& other); /*Copy constructor: It creates a new object by copying an existing one. */ 
        Channel& operator=(const Channel& other); /*Copy assignment operator: It copies the contents of one existing object into another already existing object. */
        ~Channel(); /*Destructor*/

        /* User management*/
        void addAdmin(Client* client);
        void addUser(Client* client);
        void removeUser(Client* client);
        void removeAdmin(Client* client);

        /* Getters (Access information): they return internal state values from private attributes. They don't modify the object, read-only access */
        const std::string& getName() const;
        const std::string& getTopic() const;
        const std::string& getPassword() const;
        const std::string& getLimit() const;
        const std::vector<Client*>& getAdmins() const;
        const std::vector<Client*>& getRegularUsers() const;
        bool isPrivate() const;
        bool isInviteOnly() const;
        bool isFreeTopic() const;

        /* Setters (Change information) */
        void setChannelTopic(const std::string& topic);
        void setInviteOnly(bool invite);   
        void setPrivate(bool priv, std::string pass);
        void setFreeTopic(bool priv);
        void setLimit(const std::string limit);

    private:
        std::string _name; /* Channel name */
        std::string _pass; /* Channel password */
        bool _private; /* Is the channel private? */
        bool _freetopic; /* Is the channel topic free to change? */
        bool _invite; /* Is the channel invite-only? */
        std::vector<Client*> _adminUsers; /* List of admin users */
        std::vector<Client*> _regularUsers; /* List of regular users */
        std::string _topic; /* Channel topic */
        std::string _limit; /* Channel user limit */
};

#endif
