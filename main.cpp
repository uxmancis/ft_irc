#define PORT 6667

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>           // close(), gethostname
#include <arpa/inet.h>        // inet_ntoa
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>            // gethostbyname

std::string get_local_ip(void) 
{
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == -1)
       return (std::cerr << "Error geting Hostame" << std::endl, "");
    hostent* host = gethostbyname(hostname);
    if (!host || !host->h_addr_list[0]) 
        return (std::cerr << "Error geting pubilc IP" << std::endl, "");
    return (inet_ntoa(*(struct in_addr*)host->h_addr_list[0]));
}

int main(int argc, char **argv) 
{
    if (argc == 3)
    {
        // 1. Crear socket
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) 
            return (std::cerr << "Error creating socket" << std::endl, 1);

        // 2. Reusar dirección
        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        // 3. Obtener IP local automáticamente
        std::string ip_str = get_local_ip();
        if (ip_str.empty()) 
        {
            std::cerr << "No se pudo obtener la IP local.\n";
            close(server_fd);
            return (1);
        }
        // 4. Preparar sockaddr_in con IP local
        sockaddr_in server_addr; {};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port   = htons(atoi(argv[1]));
        server_addr.sin_addr.s_addr = inet_addr(ip_str.c_str());

        std::cerr << "Configurado en IP: " << ip_str << "\n";

        // 5. Bind
        if (bind(server_fd, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) < 0) 
        {
            std::cerr << "Error Bind" << std::endl;
            close(server_fd);
            return (1);
        }
        std::cout << "Bind exitoso\n";

        // 6. Listen
        if (listen(server_fd, SOMAXCONN) < 0) 
        {
            std::cerr << "Error Listening" << std::endl;
            close(server_fd);
            return (1);
        }

        std::cout << "Escuchando conexiones en " << ip_str << ":" << argv[1] << "\n";
        std::cout << "Presiona ENTER para salir.\n";
        std::cin.get();

        close(server_fd);
    }
    std::cerr << "Usage: ./ircserv [PORT] [PASSWORD]" << std::endl;
    return (1);
}
