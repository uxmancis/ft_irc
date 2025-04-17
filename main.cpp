#define PORT 6667

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 6667

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>           // close()
#include <arpa/inet.h>        // inet_addr, inet_ntoa
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 6667

int main() {
    // 1. Crear socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) 
    {
        perror("socket");
        return 1;
    }

    // 2. Reusar dirección
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 3. Preparar sockaddr_in para la IP específica
    sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;            // IPv4
    server_addr.sin_port   = htons(PORT);        // Puerto en orden de red

    // -> EN VEZ DE inet_pton, usamos inet_addr:
    server_addr.sin_addr.s_addr = inet_addr("10.14.8.6");
    if (server_addr.sin_addr.s_addr == INADDR_NONE) 
    {
        std::cerr << "inet_addr: IP inválida\n";
        close(server_fd);
        return 1;
    }

    // (Opcional) mostrar la IP y el puerto configurados
    std::cout << "Configurado en IP: " << inet_ntoa(server_addr.sin_addr)  << std::endl;

    // 4. Bind
    if (bind(server_fd, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) < 0) 
    {
        perror("bind");
        close(server_fd);
        return 1;
    }

    std::cout << "Bind exitoso" << std::endl;

    // 5. Listen
    if (listen(server_fd, SOMAXCONN) < 0) 
    {
        perror("listen");
        close(server_fd);
        return 1;
    }

    std::cout << "Escuchando conexiones en " << inet_ntoa(server_addr.sin_addr) << ":" << PORT << std::endl;

    // 6. Pausa para prueba manual
    std::cout << "Presiona ENTER para salir.\n";
    std::cin.get();

    close(server_fd);
    return 0;
}
