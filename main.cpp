#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <poll.h>

#include <iostream>
#include <cstring>
#include <unistd.h>         // close()
#include <arpa/inet.h>      // htons, inet_addr
#include <sys/socket.h>
#include <netinet/in.h>     // sockaddr_in

#define PORT 6667

int main() {
    // 1. Crear el socket (IPv4, TCP)
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return 1;
    }
    std::cout << "Socket creado! FD: " << server_fd << "\n";

    // 2. Configurar dirección del servidor
    sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;              // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;      // 0.0.0.0 (todas las interfaces)
    server_addr.sin_port = htons(PORT);            // Puerto 6667 (en orden de red)

    // 3. Reutilizar el puerto si fue usado recientemente
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 4. Hacer bind() → asocia socket con dirección + puerto
    if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        return 1;
    }
    std::cout << "Bind exitoso. Puerto: " << PORT << "\n";

    // 5. Poner socket en modo escucha
    if (listen(server_fd, SOMAXCONN) < 0) {
        perror("listen");
        close(server_fd);
        return 1;
    }
    std::cout << "Escuchando conexiones en puerto " << PORT << "...\n";

    // Esperar para que puedas probar con telnet
    std::cout << "Presiona ENTER para salir.\n";
    std::cin.get();

    close(server_fd);
    return 0;
}
