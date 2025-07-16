#include "Client.hpp"

Client::Client() : _clientFD(-1), _nickname(""), _username(""), _actualGroup("#general"), _pass(""), _state(AWAITING_PASSWORD), _hasJoinedGeneral(false), _previousPING(-1), _receivedPONG(false) {}

Client::Client(int clientFD) : _clientFD(clientFD), _nickname(""), _username(""), _actualGroup("#general"), _pass(""), _state(AWAITING_PASSWORD), _hasJoinedGeneral(false), _previousPING(-1), _receivedPONG(false) {}

Client::~Client() {}

Client::Client(const Client &other) : _clientFD(other._clientFD), _nickname(other._nickname), _username(other._username), _actualGroup(other._actualGroup), _pass(other._pass), _state(other._state), _hasJoinedGeneral(other._hasJoinedGeneral), _previousPING(other._previousPING), _receivedPONG(other._receivedPONG) {}

Client &Client::operator=(const Client &other)
{
    if (this != &other)
    {
        _clientFD = other._clientFD;
        _nickname = other._nickname;
        _username = other._username;
        _actualGroup = other._actualGroup;
        _pass = other._pass;
        _state = other._state;
        _hasJoinedGeneral = other._hasJoinedGeneral;
        _receivedPONG = other._receivedPONG;
        _previousPING = other._previousPING;
    }
    return *this;
}

int Client::getClientFD() const                             { return _clientFD; }
const std::string &Client::getNickname() const              { return _nickname; }
const std::string &Client::getUsername() const              { return _username; }
const std::string &Client::getActualGroup() const           { return _actualGroup; }
const std::string &Client::getPassword() const              { return _pass; }
ClientState Client::getState() const                        { return _state; }
bool Client::hasJoinedGeneral() const                       { return _hasJoinedGeneral; }
time_t Client::getPreviousPING() const                      { return _previousPING; }
bool Client::hasReceivedPONG() const                        { return _receivedPONG; }


void Client::setNickname(const std::string &nickname)       { _nickname = nickname; }
void Client::setUsername(const std::string &username)       { _username = username; }
void Client::setActualGroup(const std::string &group)       { _actualGroup = group; }
void Client::setState(ClientState state)                    { _state = state; }
void Client::setPassword(const std::string &pass)           { _pass = pass; }
void Client::setJoinedGeneral(bool value)                   { _hasJoinedGeneral = value; }
void Client::setPreviousPING(time_t previousPING)           { _previousPING = previousPING; }
void Client::setReceivedPONG(bool value)                    { _receivedPONG = value; }