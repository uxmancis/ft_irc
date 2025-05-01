#ifndef CLIENT_HPP
#define CLIENT_HPP

# include <vector>
# include <string>
# include <map>
# include <poll.h>

enum ClientState 
{
    AWAITING_PASSWORD,
    AWAITING_NICKNAME,
    AWAITING_USERNAME,
    READY
};

class Client 
{
    public:
        Client();                          // Default constructor
        Client(int clientFD);              // Constructor con el FD
        ~Client();                         // Destructor

        Client(const Client& other);       // Copy constructor
        Client& operator=(const Client&);  // Copy assignment

        // Getters
        int                 getClientFD() const;
        const std::string&  getNickname() const;
        const std::string&  getUsername() const;
        const std::string&  getActualGroup() const;
        ClientState         getState() const;

        // Setters
        void                setNickname(const std::string& nickname);
        void                setUsername(const std::string& username);
        void                setActualGroup(const std::string& group);
        void                setState(ClientState state);

    private:
        int             _clientFD;
        std::string     _nickname;
        std::string     _username;
        std::string     _actualGroup;
        ClientState     _state;

};

#endif
