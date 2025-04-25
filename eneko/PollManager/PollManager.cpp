#include "PollManager.hpp"

PollManager::PollManager(int serverFD, std::string password) : _password(password), _serverFD(serverFD)
{
    pollfd pfd;
    pfd.fd = _serverFD;
    pfd.events = POLLIN;
    _fds.push_back(pfd);
}

void PollManager::AcceptNewUser(void)
{
    sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int client_fd = accept(_serverFD, (sockaddr*)&client_addr, &addr_len);
    if (client_fd < 0)
    {
        return;
    }

    // Obtener IP como string
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, ip_str, sizeof(ip_str));
    std::cout << "Nueva conexión desde IP: " << ip_str << std::endl;

    // Pedir contraseña
    const std::string prompt = "Introduce la contraseña: ";
    send(client_fd, prompt.c_str(), prompt.size(), 0);

    // Leer contraseña enviada por el cliente
    char buffer[_password.size() + 10000];
    int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0)
    {
        close(client_fd);
        return;
    }
    buffer[bytes] = '\0';
    std::string pass = buffer;

    // Limpiar posible salto de línea
    if (!pass.empty() && pass[pass.length() - 1] == '\n')
        pass.erase(pass.length() - 1);
    if (!pass.empty() && pass[pass.length() - 1] == '\r')
        pass.erase(pass.length() - 1);

    // Verificar contraseña
    if (pass != _password)
    {
        const std::string fail_msg = "Contraseña incorrecta\n";
        send(client_fd, fail_msg.c_str(), fail_msg.size(), 0);
        close(client_fd);
        return;
    }
    std::string success_msg = "Bienvenido a nuestro servidor IRC\n";
    send(client_fd, success_msg.c_str(), success_msg.size(), 0);
    success_msg = "Set your IRC nickname\n";
    send(client_fd, success_msg.c_str(), success_msg.size(), 0);
    bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0)
    {
        close(client_fd);
        return;
    }
    buffer[bytes] = '\0';
    std::string nickname_buffer = buffer;
    if (!nickname_buffer.empty() && nickname_buffer[nickname_buffer.length() - 1] == '\n')
    nickname_buffer.erase(nickname_buffer.length() - 1);
    if (!nickname_buffer.empty() && nickname_buffer[nickname_buffer.length() - 1] == '\r')
    nickname_buffer.erase(nickname_buffer.length() - 1);
    
    success_msg = "Set your IRC username\n";
    send(client_fd, success_msg.c_str(), success_msg.size(), 0);
    bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0)
    {
        close(client_fd);
        return;
    }
    buffer[bytes] = '\0';
    std::string username_buffer = buffer;
    if (!username_buffer.empty() && username_buffer[username_buffer.length() - 1] == '\n')
    username_buffer.erase(username_buffer.length() - 1);
    if (!username_buffer.empty() && username_buffer[username_buffer.length() - 1] == '\r')
    username_buffer.erase(username_buffer.length() - 1);
    
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
    Client new_client(client_fd, nickname_buffer, username_buffer);
    _client.push_back(new_client);
    pollfd pfd;
    pfd.fd      = client_fd;
    pfd.events  = POLLIN;
    pfd.revents = 0;
    _fds.push_back(pfd);
}

void PollManager::HandleNewMsg(int fd)
{
    char buffer[10000];
    int bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) 
    {
        close(fd);
        return;
    }
    buffer[bytes] = '\0';
    std::string prefix;
    for (size_t i = 0; i < _client.size(); i++)
    {
        if (_client[i]._client_fd == fd && _fds[i].fd != fd)
        {
            prefix = std::string(RED) + "[General] " + GREEN "<" + _client[i]._nickname + "> " + WHITE;
        }
    }
    for (size_t i = 0; i < _fds.size(); i++) //-> check all file descriptors
    {
        if (_fds[i].fd != _serverFD && _fds[i].fd != fd)
        {
            send(_fds[i].fd, prefix.c_str(), prefix.size(), 0);
            send(_fds[i].fd, buffer, bytes, 0);
        }
    }
}

void PollManager::run(void)
{
    std::cout << "Esperando conexiones...\n";
    while (true) 
    {
        if((poll(&_fds[0],_fds.size(),-1) == -1)) //-> wait for an event
            throw(std::runtime_error("poll() faild"));

        for (size_t i = 0; i < _fds.size(); i++) //-> check all file descriptors
        {
            if (_fds[i].revents & POLLIN)//-> check if there is data to read
            {
                if (_fds[i].fd == _serverFD)
                    AcceptNewUser();
                else
                HandleNewMsg(_fds[i].fd);
            }
        }
        
    }
}