#include "../inc/ircserv.hpp"

/* 
* IRC server: a program that waits for a client to connect
* via TCP socket on a specific por (6667, standard for IRC).
*/

/*
* What is a socket? 
*
*

* IPv4: Internet Protocol version 4
*
* 👀 man socket
* int socket(int domain, int type, int protocol);
*        > domain: Address family (AF_INET for IPv4, e.g. 192.168.1.1)
*        > type: Type of socket (SOCK_STREAM for TCP), how data will be sent.
*                SOCK_STREAM: reliable, two-way, connection-based (TCP)
*        > protocol: Protocol to be used (0 for default)
*                SOCK_STREAM: uses TCP protocol as default (our)
*                SOCK_DGARAM: uses UDP protocol as default
*        > return: file descriptor for the socket
*
* Objective: to create a socket for communication
* What is a socket?
* A socket is an endpoint for communication between two machines.
* It is a combination of an IP address and a port number.
* A socket is a software structure that allows communication between two processes.
* It is a way to send and receive data over a network.
* A socket is a way to communicate between two machines over a network.
* 
*/
int get_socket()
{
	int serverSocket;

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    //printf("socket: %d\n", serverSocket);
	if (serverSocket == -1)
	{
        std::cerr << "Error: failed to create socket" << std::endl;
        return (EXIT_FAILURE);
    }
	return (serverSocket);
}

//std::cerr is used to output error messages, instead of std::cout
int check_exec_input(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << RED "Incorrect input." WHITE " Try again: " << argv[0] << " <port> <password>\n" << std::endl;
        return (EXIT_FAILURE);
    }
    return (0);
}

/* 127.0.0.1 = "This computer"
*
* Socket 
*/
int main(int argc, char **argv)
{
    int serverSocket;
    int port = 7777;

    check_exec_input(argc, argv);
    // Step 1: Create the socket
	serverSocket = get_socket();
    printf("argc: %d\n", argc);
    (void)argv;
		
    // Step 2: Set up the server address struct
    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    serverAddr.sin_port = htons(port);

    // Step 3: Bind the socket to the port
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Error: bind failed" << std::endl;
        close(serverSocket);
        return 1;
    }

    // Step 4: Start listening
    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Error: listen failed" << std::endl;
        close(serverSocket);
        return 1;
    }
    std::cout << "Server is listening on port " << port << "..." << std::endl;

    // Step 5: Accept a connection (blocking)
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
    if (clientSocket < 0) {
        std::cerr << "Error: accept failed" << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "New client connected from "
              << inet_ntoa(clientAddr.sin_addr) << std::endl;

    // Step 6: Close sockets (clean up)
    close(clientSocket);
    close(serverSocket);

    return 0;
}
