#define PORT 6667

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>           // close(), gethostname
#include <arpa/inet.h>        // inet_ntoa
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>            // gethostbyname

std::string get_local_ip() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == -1) {
        perror("gethostname");
        return "";
    }

    hostent* host = gethostbyname(hostname);
    if (!host || !host->h_addr_list[0]) {
        perror("gethostbyname");
        return "";
    }

    return inet_ntoa(*(struct in_addr*)host->h_addr_list[0]);
}

int main() {
    // 1. Crear socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    // 2. Reusar dirección
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 3. Obtener IP local automáticamente
    std::string ip_str = get_local_ip();
    if (ip_str.empty()) {
        std::cerr << "No se pudo obtener la IP local.\n";
        close(server_fd);
        return 1;
    }

    // 4. Preparar sockaddr_in con IP local
    sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(ip_str.c_str());

    std::cout << "Configurado en IP: " << ip_str << "\n";

    // 5. Bind
    if (bind(server_fd, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    std::cout << "Bind exitoso\n";

    // 6. Listen
    if (listen(server_fd, SOMAXCONN) < 0) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    std::cout << "Escuchando conexiones en " << ip_str << ":" << PORT << "\n";
    std::cout << "Presiona ENTER para salir.\n";
    std::cin.get();

    close(server_fd);
    return 0;
}
