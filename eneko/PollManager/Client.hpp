#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "PollManager.hpp"

class  Client
{
    public:
        Client(int client_fd, std::string nickname, std::string username) : _client_fd(client_fd), _nickname(nickname), _username(username) {};
        int _client_fd;
        std::string _nickname;
        std::string _username;

};


#endif