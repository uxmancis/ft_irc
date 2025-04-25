#include "PollManager.hpp"

PollManager::PollManager(int serverFD, std::string password) : _password(password), _serverFD(serverFD)
{
    pollfd pfd;
    pfd.fd = _serverFD;
    pfd.events = POLLIN;
    _fds.push_back(pfd);
}

void PollManager::trimNewlines(std::string& str)
{
    if (!str.empty() && str[str.length() - 1] == '\n')
        str.erase(str.length() - 1);
    if (!str.empty() && str[str.length() - 1] == '\r')
        str.erase(str.length() - 1);
}

std::string PollManager::requestInput(int client_fd, const std::string& msg)
{
    send(client_fd, msg.c_str(), msg.size(), 0);
    char buffer[512];
    int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0)
        return "";
    buffer[bytes] = '\0';
    std::string input(buffer);
    trimNewlines(input);
    return input;
}

bool PollManager::authenticateClient(int client_fd)
{
    const std::string prompt = "Introduce la contraseña: ";
    send(client_fd, prompt.c_str(), prompt.size(), 0);

    char buffer[_password.size() + 512];
    int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0)
        return false;

    buffer[bytes] = '\0';
    std::string input = buffer;
    trimNewlines(input);
    if (input != _password)
    {
        const std::string msg = "Contraseña incorrecta\n";
        send(client_fd, msg.c_str(), msg.size(), 0);
        return false;
    }
    return true;
}

void PollManager::AcceptNewUser()
{
    sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_fd = accept(_serverFD, (sockaddr*)&client_addr, &addr_len);
    if (client_fd < 0)
        return;
    
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, ip_str, sizeof(ip_str));
    std::cout << "[INFO] Nueva conexión desde IP: " << ip_str << std::endl;
    
    if (!authenticateClient(client_fd))
    {
        close(client_fd);
        return;
    }
    
    send(client_fd, "Bienvenido a nuestro servidor IRC\n", 35, 0);
    std::string nickname = requestInput(client_fd, "Introduce tu IRC nickname\n");
    std::string username = requestInput(client_fd, "Introduce tu IRC username\n");
    _mapGroup["General"].push_back(_client.size());
    send(client_fd, "Buenas, te encuentras en el canal [General]\n", 45, 0);
    
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
    Client new_client(client_fd, nickname, username);
    _client.push_back(new_client);

    pollfd pfd;
    pfd.fd = client_fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _fds.push_back(pfd);

    std::cout << "[AUTH] Cliente " << client_fd << " autenticado como " << nickname << std::endl;
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
    if (bytes == 2)
    {
        std::string clear_line = "\033[F\033[2K"; 
        send(fd, clear_line.c_str(), clear_line.size(), 0);
        return ;
    }
    buffer[bytes] = '\0';
    std::string help = buffer;
    if (!help.empty() && help[help.length() - 1] == '\n')
        help.erase(help.length() - 1);
    if (!help.empty() && help[help.length() - 1] == '\r')
        help.erase(help.length() - 1);
    
    if (help == "/HELP" || help == "/help")
    {
        send(fd, YELLOW "OPCIONES: \n", 12 + strlen(YELLOW), 0);
        send(fd, "/JOIN: unirse a un grupo, si no existe se crea \n", 49, 0);
        send(fd, "/ILUSTRATE: muestra grupos y usuarios conectados \n", 51, 0);
        send(fd, "/PRIVATE: enviar mensajes privados a los usuarios conectados \n" RESET, 63 + strlen(RESET), 0);
        return ;
    } 
    if (!help.compare(0, 5, "/JOIN ", 0, 5))
    {
        std::string _group = help.substr(5);
        if (_group.empty())
        {
            send(fd, RED "Después de /JOIN pon el nombre del grupo que quieres crear o unirte\n" RESET, 70 + strlen(RED) + strlen(RESET), 0);
            return;
        }
        _group = _group.substr(1);
        if (_group.empty())
        {
            send(fd, RED "Después de /JOIN pon el nombre del grupo que quieres crear o unirte\n" RESET, 70 + strlen(RED) + strlen(RESET), 0);
            return;
        }
        size_t i = 0;
        while (i < _client.size() && fd != _client[i]._client_fd)
            i++;
        if (i == _client.size())
            return ;
        if (_mapGroup.find(_group) == _mapGroup.end()) {
            _mapAdmin[_group].push_back(i);
            _mapGroup[_group].push_back(i);
            _client[i]._actual_group = _group;
            std::string buffer = BLUE "GRUPO CREADO Y UNIDO COMO ADMIN " + help + "\n" RESET;
            send(fd, buffer.c_str() , buffer.size(), 0);
        }
        else 
        {
            std::vector<int>& groupMembers = _mapGroup[_group];
            if (std::find(groupMembers.begin(), groupMembers.end(), i) == groupMembers.end())
                groupMembers.push_back(i);
            _client[i]._actual_group = _group;
            std::string buffer = BLUE "TE UNISTE AL GRUPO EXISTENTE " + help + "\n" RESET;
            send(fd, buffer.c_str() , buffer.size(), 0);
    
        }
        return;
    }
    if (!help.compare(0, 11, "/ILUSTRATE", 0, 11))
    {
        std::string output = "\n[Grupos y Admins]\n";
        for (std::map<std::string, std::vector<int> >::const_iterator it = _mapAdmin.begin(); it != _mapAdmin.end(); ++it) 
        {
            output += "👥Grupo: " + it->first + "\n";
            output += " 🔑Admins:\n";
            for (size_t i = 0; i < it->second.size(); ++i)
                output += RED "     -" + _client[it->second[i]]._nickname + "\n" RESET;
        }

        output += "\n[Grupos y Miembros]\n";
        for (std::map<std::string, std::vector<int> >::const_iterator it = _mapGroup.begin(); it != _mapGroup.end(); ++it) {
            output += "👥Grupo: " + it->first + "\n";
            output += "  Miembros:\n";
            for (size_t i = 0; i < it->second.size(); ++i)
                output += CYAN "    - " + _client[it->second[i]]._nickname + "\n" RESET;
        }
        send(fd, output.c_str(), output.length(), 0);
        return;
    } 
    std::string msg(buffer);
    std::string prefix;
    std::string prefix1;
    size_t x = 0;
    for (;x < _client.size(); ++x)
    {
        if (_client[x]._client_fd == fd)
        {
            prefix = RED "[" + _client[x]._actual_group + "]" GREEN " <" + _client[x]._nickname + "> " RESET;
            prefix1 = RED "[" + _client[x]._actual_group + "]" CYAN " <yo> " RESET;
            break;
        }
    }

    std::string groupName = _client[x]._actual_group;
    std::map<std::string, std::vector<int> >::const_iterator it = _mapGroup.find(groupName);
    if (it != _mapGroup.end())
    {
        const std::vector<int>& groupMembers = it->second;
        std::string full_message = prefix + msg;

        for (size_t i = 0; i < groupMembers.size(); ++i)
        {
            int target_fd = _client[groupMembers[i]]._client_fd;

            if (target_fd != _serverFD && target_fd != fd) // no se lo mandes al server ni al emisor
                send(target_fd, full_message.c_str(), full_message.size(), 0);
        }
    }
    std::string clear_line = "\033[F\033[2K";
    send(fd, clear_line.c_str(), clear_line.size(), 0);
    std::string full_message = prefix1 + msg;
    send(fd, full_message.c_str(), full_message.size(), 0);
    send(fd, "\r", 1, 0);  
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