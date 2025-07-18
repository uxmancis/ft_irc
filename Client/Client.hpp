#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <vector>
# include <string>
# include <map>
# include <poll.h>
# include <ctime>


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
        Client();
        Client(int clientFD);
        ~Client();

        Client(const Client& other);
        Client& operator=(const Client&);

        int                 getClientFD() const;
        const std::string&  getNickname() const;
        const std::string&  getUsername() const;
        const std::string&  getActualGroup() const;
        const std::string&  getPassword() const;
        ClientState         getState() const;
        time_t              getPreviousPING() const;
        bool                hasJoinedGeneral() const;
        bool                hasReceivedPONG() const;


        void                setNickname(const std::string& nickname);
        void                setUsername(const std::string& username);
        void                setActualGroup(const std::string& group);
        void                setPassword(const std::string& pass);
        void                setState(ClientState state);
        void                setJoinedGeneral(bool value);
        void                setPreviousPING(time_t previousPING);
        void                setReceivedPONG(bool value);


    private:    
        int                 _clientFD;
        std::string         _nickname;
        std::string         _username;
        std::string         _actualGroup;
        std::string         _pass;
        ClientState         _state;
        bool                _hasJoinedGeneral;
        time_t              _previousPING;
        bool                _receivedPONG;

};

#endif
