#include "Commands.hpp"
#include <sstream>
#include <vector>

void HandleHELP(int fd)
{
    std::string help =
        "\033[32m[Comandos disponibles]\033[0m\n"
        "/HELP             - Muestra esta ayuda\n"
        "/JOIN <canal>     - Unirse o crear un canal\n"
        "/NICK <nombre>    - Cambiar tu nickname\n"
        "/USER <usuario>   - Cambiar tu nombre de usuario\n"
        "/MSG <texto>      - Enviar mensaje al canal actual\n"
        "/ILUSTRATE        - Muestra Canales y Usuarios Conectados\n"
        "\n";
    send(fd, help.c_str(), help.size(), 0);
}
