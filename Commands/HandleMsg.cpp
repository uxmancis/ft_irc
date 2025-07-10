#include "Commands.hpp"
#include <sstream>
#include <vector>


/* void HandleMSG(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    std::cout << "--------------------- Inside HandleMSG ---------------------" << std::endl;
    
    // 1. Validar que haya al menos dos argumentos: destinatario y mensaje
    if (args.size() < 2) {
        std::string err = "Uso: /MSG <Usuario> <Mensaje>\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }

    // 2. Obtener información del cliente emisor
    Client& sender = pollManager.getClient(fd);
    std::string senderNick = sender.getNickname();
    std::string senderUser = sender.getUsername();
    std::string senderHost = "localhost"; // o usa sender.getHostname() si lo tienes

    // 3. Reconstruir el mensaje desde args[1] en adelante
    std::string msg;
    for (size_t i = 1; i < args.size(); ++i) {
        msg += args[i];
        if (i != args.size() - 1)
            msg += " ";
    }

    // 4. Buscar al cliente destinatario por su nickname
    const std::map<int, Client>& clients = pollManager.getClients();
    Client* recipient = NULL;

    std::map<int, Client>::const_iterator it;
    for (it = clients.begin(); it != clients.end(); ++it) 
    {
        if (it->second.getNickname() == args[0]) {
            recipient = const_cast<Client*>(&it->second); // cuidado: solo si no puedes modificar el map
            break;
        }
    }

    // 5. Si no se encuentra el destinatario, enviar error al emisor
    if (recipient == NULL) 
    {
        std::string err = "El usuario no existe\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }

    std::cout << msg << std::endl;

    const std::string& target = args[0];

    // 4. Si es un canal
    if (target[0] == '#') 
    {
        std::string channelName = target;
    
        // 1. Comprobar si el canal existe
        std::map<std::string, Channel>& channels = pollManager.getChannels();
        if (channels.find(channelName) == channels.end()) 
        {
            std::string err = ":irc.local 403 " + channelName + " :No such channel\r\n";
            send(fd, err.c_str(), err.size(), 0);
            return;
        }
    
        Channel& channel = channels[channelName];
    
        // 2. Verificar que el cliente esté en el canal
        if (!channel.hasUser(&pollManager.getClient(fd)))  // Suponiendo que Channel tiene un método hasUser(int fd)
        {
            std::string err = ":irc.local 442 " + channelName + " :You're not on that channel\r\n";
            send(fd, err.c_str(), err.size(), 0);
            return;
        }
    
        // 3. Construir el mensaje
        std::string ircMessage = ":" + senderNick + "!" + senderUser + "@" + senderHost +
                                 " PRIVMSG " + channelName + " :" + msg + "\r\n";
    
        // 4. Enviar a todos los miembros del canal, menos al emisor
        const std::vector<Client* > &users = channel.getRegularUsers();  // o getUsers() si tienes todos
        for (std::vector<Client*>::const_iterator it = users.begin(); it != users.end(); ++it) 
        {
            Client* recipient = *it;
            if (recipient->getClientFD() == fd)
                continue; // No le mandes el mensaje a ti mismo

            send(recipient->getClientFD(), ircMessage.c_str(), ircMessage.size(), 0);
        }

        // 5. (Opcional) Feedback al emisor
        std::string feedback = "[yo]-> " + channelName + " " + msg + "\n";
        send(fd, feedback.c_str(), feedback.size(), 0);
    }    
    else
    {
        // 6. Construir el mensaje IRC PRIVMSG
        std::string ircMessage = ":" + senderNick + "!" + senderUser + "@" + senderHost +
                                " PRIVMSG " + recipient->getNickname() + " :" + msg + "\r\n";

        // 7. Enviar el mensaje privado al destinatario
        send(recipient->getClientFD(), ircMessage.c_str(), ircMessage.size(), 0);

        // 8. (Opcional) Mostrar al emisor su propio mensaje como feedback
        std::string feedback = "[yo]-> " + recipient->getNickname() + " " + msg + "\n";
        send(fd, feedback.c_str(), feedback.size(), 0);
    }
}

#include "Commands.hpp"
#include <sstream>
#include <vector> */

void HandleMSG(int fd, const std::vector<std::string>& args, PollManager& pollManager)
{
    std::cout << "--------------------- Inside HandleMSG ---------------------" << std::endl;

    if (args.size() < 2) {
        std::string err = ":irc.local 461 * PRIVMSG :Not enough parameters\r\n";
        send(fd, err.c_str(), err.size(), 0);
        return;
    }

    Client& sender = pollManager.getClient(fd);
    std::string senderNick = sender.getNickname();
    std::string senderUser = sender.getUsername();
    std::string senderHost = "localhost";

    // Reconstruir el mensaje desde args[1] en adelante
    std::string msg;
    if (args[1][0] == ':') {
        msg = args[1].substr(1);  // quitar ':'
        for (size_t i = 2; i < args.size(); ++i) {
            msg += " " + args[i];
        }
    } else {
        for (size_t i = 1; i < args.size(); ++i) {
            msg += args[i];
            if (i != args.size() - 1)
                msg += " ";
        }
    }

    const std::string& target = args[0];

    if (target[0] == '#') 
    {
        std::map<std::string, Channel>& channels = pollManager.getChannels();
        std::string channelName = target;

        if (channels.find(channelName) == channels.end()) {
            std::string err = ":irc.local 403 " + channelName + " :No such channel\r\n";
            send(fd, err.c_str(), err.size(), 0);
            return;
        }

        Channel& channel = channels[channelName];

        if (!channel.hasUser(&sender)) {
            std::string err = ":irc.local 442 " + channelName + " :You're not on that channel\r\n";
            send(fd, err.c_str(), err.size(), 0);
            return;
        }

        std::string ircMessage = ":" + senderNick + "!" + senderUser + "@" + senderHost +
                                 " PRIVMSG " + channelName + " :" + msg + "\r\n";

        const std::vector<Client*>& users = channel.getRegularUsers();
        for (std::vector<Client*>::const_iterator it = users.begin(); it != users.end(); ++it) 
        {
            Client* recipient = *it;
            if (recipient->getClientFD() == fd)
                continue;
            send(recipient->getClientFD(), ircMessage.c_str(), ircMessage.size(), 0);
        }

        std::string feedback = "[yo]-> " + channelName + " " + msg + "\n";
        send(fd, feedback.c_str(), feedback.size(), 0);
    } 
    else 
    {
        const std::map<int, Client>& clients = pollManager.getClients();
        Client* recipient = NULL;

        for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
            if (it->second.getNickname() == target) 
            {
                recipient = const_cast<Client*>(&it->second);
                break;
            }
        }

        if (recipient == NULL) 
        {
            std::string err = ":irc.local 401 " + target + " :No such nick\r\n";
            send(fd, err.c_str(), err.size(), 0);
            return;
        }

        std::string ircMessage;
        /* Visualizacion para RECEPTOR  */
        ircMessage = ":" + senderNick + "!" + senderUser + "@" + senderHost + " PRIVMSG " + recipient->getNickname() + " :" + msg + "\r\n";
        send(recipient->getClientFD(), ircMessage.c_str(), ircMessage.size(), 0);
    }
}
