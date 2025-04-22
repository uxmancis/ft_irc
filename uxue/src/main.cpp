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
* 👀 SOCKET creation = "Hey Operating System, I want to create a new 
* communication endpoint using IPv4 and TCP."
*
* int socket(int domain, int type, int protocol);
*        > domain: Address family (AF_INET for IPv4, e.g. 192.168.1.1)
*        > type: Type of socket (SOCK_STREAM for TCP), how data will be sent.
*                SOCK_STREAM: reliable, two-way, connection-based (TCP)
*        > protocol: Protocol to be used (0 for default)
*                SOCK_STREAM: uses TCP protocol as default (our)
*                SOCK_DGARAM: uses UDP protocol as default
*        > return: file descriptor for the socket
*
* At this point, the socket is just a concept, represented by a fd. Still 
* not connected to anything (no IP address or port). T
* 
*/
int create_socket()
{
	int serverSocket;

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
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
        printf("helo\n");
    }
    return (0);
}

/* 127.0.0.1 = "This computer"
*
* Socket e.g.: 23.43.115.12:6250 (IP Address:Port)
* How to create a server:
*
*       1) socket function: Create socket, get fd
*       2) bind function: Bind socket to port (e.g. 6667)
*       3) listen function: Listen for incoming connections
*       4) accept function: Accept incoming connection
*       5) read/write function: Read/write data to/from socket
*       6) close function: Close socket
*       7) exit function: Exit program
*       8) free function: Free memory
*       9) shutdown function: Shutdown socket
*       10) select function: Monitor multiple sockets
*       11) poll function: Monitor multiple sockets
*       12) epoll function: Monitor multiple sockets
*       13) getsockname function: Get socket name
*       14) getpeername function: Get peer name
*/
int main(int argc, char **argv)
{
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    int port = 6774;

    //check_exec_input(argc, argv);
    // Step 1: Create the socket
	//socketfd = create_socket();
    // printf("argc: %d\n", argc);
    (void)argv;
    (void)argc;
		
    // Step 2: Set up the server address struct
    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET; //2 bytes (AF:INET for IPv4)
    serverAddr.sin_addr.s_addr = inet_addr("0.0.0.0"); //4 bytes (IPv4 address)
    serverAddr.sin_port = htons(port); //2 bytes (e.g. 8080, but in networ byte order)
    printf("****serverAddr.sin_family: %d\n", serverAddr.sin_port);
    printf("****serverAddr.sin_addr.s_addr: %d\n", serverAddr.sin_port);
    printf("****serverAddr.sin_port: %d\n", serverAddr.sin_port);

    // Step 3: Bind the socket to the port
    /*  int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
            > sockfd: Socket file descriptor
            > addr: Pointer to the address structure
            > addrlen: Size of the address structure
            > return: 0 on success, -1 on error
    */
    if (bind(socketfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Error: bind failed" << std::endl;
        close(socketfd);
        return 1;
    }

    // Get actual socket info
    struct sockaddr_in boundAddr;
    socklen_t len = sizeof(boundAddr);
    if (getsockname(socketfd, (struct sockaddr*)&boundAddr, &len) == -1) {
        perror("getsockname failed");
        return 1;
    }

    // Convert IP to readable string
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(boundAddr.sin_addr), ipStr, sizeof(ipStr));

    // Convert port to host byte order
   port = ntohs(boundAddr.sin_port);

    // Print with printf
    printf("Socket bound to IP: %s, Port: %d\n", ipStr, port);

    // Step 4: Start listening for incoming connections, makes it a passive socket (information endpoint)
    //int listen(int sockfd, int backlog);
    //Backlog: maximum possible queue, number of potential client connections waiting to be accepted
    if (listen(socketfd, 5) < 0) {
        std::cerr << "Error: listen failed" << std::endl;
        close(socketfd);
        return 1;
    }
    std::cout << "Server is listening on port " << port << "..." << std::endl;

    // Step 5: Accept a connection (blocking)
    /* Every new connection, every new client request accepted results in
    a new socket*/
    /* In TCP/IP communication, each connection is uniquely identified by
    a 4-tuple: Server IP, Server port, client IP, client port. Each client 
    that connects to a server uses a unique socket = a unique combination
    of client IP address and client port*/
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientSocket = accept(socketfd, (struct sockaddr*)&clientAddr, &clientLen);
    if (clientSocket < 0) {
        std::cerr << "Error: accept failed" << std::endl;
        close(socketfd);
        return 1;
    }

    std::cout << "New client connected from "
              << inet_ntoa(clientAddr.sin_addr) << std::endl;

    // Step 6: Close sockets (clean up)
    close(clientSocket);
    close(socketfd);

    return 0;
}
