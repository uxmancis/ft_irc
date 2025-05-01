#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>

class Server 
{
    public:
        Server(int port, const std::string& password);
        ~Server();
        Server(const Server& other);
        Server& operator=(const Server& other);

        void run();

    private:
        int         _serverFD;
        std::string _password;
        int         _port;

        void        _setupSocket();
};

#endif
