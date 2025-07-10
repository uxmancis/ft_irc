#include "Client.hpp"

Client::Client() : _clientFD(-1), _nickname(""), _username(""), _actualGroup("#general"), _state(AWAITING_PASSWORD), _hasJoinedGeneral(false) {}

Client::Client(int clientFD) : _clientFD(clientFD), _nickname(""), _username(""), _actualGroup("#general") {}

Client::~Client() {}

Client::Client(const Client& other) 
{
    _clientFD = other._clientFD;
    _nickname = other._nickname;
    _username = other._username;
    _actualGroup = other._actualGroup;
}

Client& Client::operator=(const Client& other) 
{
    if (this != &other) 
    {
        _clientFD = other._clientFD;
        _nickname = other._nickname;
        _username = other._username;
        _actualGroup = other._actualGroup;
    }
    return (*this);
}

// Getters
int Client::getClientFD() const                     { return _clientFD;     }
const std::string& Client::getNickname() const      { return _nickname;     }
const std::string& Client::getUsername() const      { return _username;     }
const std::string& Client::getActualGroup() const   { return _actualGroup;  }
const std::string& Client::getPassword() const      { return _pass;         }
ClientState Client::getState() const                { return _state;        }

// Setters
void Client::setNickname(const std::string& nickname)   { _nickname = nickname; }
void Client::setUsername(const std::string& username)   { _username = username; }
void Client::setActualGroup(const std::string& group)   { _actualGroup = group; }
void Client::setState(ClientState state)                { _state = state;       }
void Client::setPassword(const std::string& pass)       { _pass = pass;}


bool Client::hasJoinedGeneral() const { return _hasJoinedGeneral; }
void Client::setJoinedGeneral(bool value) { _hasJoinedGeneral = value; }
