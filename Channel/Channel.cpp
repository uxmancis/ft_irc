#include "Channel.hpp"
#include "iostream"
#include "sys/socket.h"

Channel::Channel()  : _name(""), _pass(""), _private(false), _freetopic(true), _invite(false), _topic("none"), _limit("") {}

Channel::Channel(std::string channelName, std::string pass, bool priv, bool invite): _name(channelName), _pass(pass), _private(priv), _freetopic(true), _invite(invite), _topic("none"), _limit("") {}

Channel::Channel(const Channel& other)
{
    _name = other._name;
    _pass = other._pass;
    _private = other._private;
    _freetopic = other._freetopic;
    _invite = other._invite;
    _adminUsers = other._adminUsers;
    _regularUsers = other._regularUsers;
    _topic = other._topic;
    _limit = other._limit;
}

Channel& Channel::operator=(const Channel& other)
{
    if (this != &other)
    {
        _name = other._name;
        _pass = other._pass;
        _private = other._private;
        _freetopic = other._freetopic;
        _invite = other._invite;
        _adminUsers = other._adminUsers;
        _regularUsers = other._regularUsers;
        _topic = other._topic;
        _limit = other._limit;
    }
    return *this;
}

Channel::~Channel() {}

void Channel::addAdmin(Client* client)
{
    for (size_t i = 0; i < _adminUsers.size(); ++i)
    {
        if (_adminUsers[i] == client)
        {
            send(client->getClientFD(), "Ya eres administrador del grupo.\n", 33, 0);
            return;
        }
    }
    _adminUsers.push_back(client);
}

void Channel::addUser(Client* client)
{
    for (size_t i = 0; i < _regularUsers.size(); ++i)
    {
        if (_regularUsers[i] == client)
        {
            send(client->getClientFD(), "Ya pertenece a ese grupo.\n", 27, 0);
            return;
        }
    }
    _regularUsers.push_back(client);
}

void Channel::removeUser(Client* client) 
{
    for (size_t i = 0; i < _regularUsers.size(); ++i) 
    {
        if (_regularUsers[i] == client) 
        {
            _regularUsers.erase(_regularUsers.begin() + i);
            return;
        }
    }
}

void Channel::removeAdmin(Client* client) 
{
    for (size_t i = 0; i < _adminUsers.size(); ++i) 
    {
        if (_adminUsers[i] == client) 
        {
            _adminUsers.erase(_adminUsers.begin() + i);
            return;
        }
    }
}

bool Channel::hasUser(Client* client) const 
{
    for (size_t i = 0; i < _regularUsers.size(); ++i) 
    {
        if (_regularUsers[i] == client)
            return true;
    }
    return false;
}

bool Channel::isFull() const 
{
    int limit = std::atoi(_limit.c_str());
    if (limit == 0)
        return false;
    return static_cast<int>(_regularUsers.size()) >= limit;
}

bool Channel::isAdmin(Client* client) const 
{
    for (size_t i = 0; i < _adminUsers.size(); ++i) {
        if (_adminUsers[i] == client)
            return true;
    }
    return false;
}

const std::string& Channel::getName() const                  { return _name; }
const std::string& Channel::getTopic() const                 { return _topic; }
const std::string& Channel::getPassword() const              { return _pass; }
const std::string& Channel::getLimit() const                 { return _limit; }
const std::vector<Client*>& Channel::getAdmins() const       { return _adminUsers; }
const std::vector<Client*>& Channel::getRegularUsers() const { return _regularUsers; }
bool Channel::isPrivate() const                              { return _private; }
bool Channel::isInviteOnly() const                           { return _invite; }
bool Channel::isFreeTopic() const                            { return _freetopic; }
void Channel::setInviteOnly(bool invite)                     { _invite = invite; }
void Channel::setPrivate(bool priv, std::string pass)        { _private = priv; _pass = pass; }
void Channel::setChannelTopic(const std::string& topic)      { _topic = topic; }
void Channel::setFreeTopic(bool freetopic)                   { _freetopic = freetopic; }
void Channel::setLimit(const std::string limit)              { _limit = limit; }