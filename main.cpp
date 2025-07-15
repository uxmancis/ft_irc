#include <iostream>
#include <cstdlib>
#include <signal.h>
#include "Server/Server.hpp"
#include "Colors.hpp"

volatile sig_atomic_t g_running = 1;

void handleSigint(int signum)
{
    (void)signum;
    std::cout << "\n[INFO] SIGINT received (Ctrl+C). Shutting down server...\n";
    g_running = 0;
}

// void handleSigint2(int signum)
// {
//     (void)signum;
//     std::cout << "\n[INFO] SIGINT received (Ctrl+Z). Shutting down client...\n";
//     sle
//     g_running = 0;
// }

int main(int argc, char** argv)
{
    if (argc != 3) 
    {
        std::cerr << RED "Usage: ./ircserv [PORT] [PASSWORD]\n" RESET;
        return 1;
    }
    int port = std::atoi(argv[1]);
    if (port <= 0 || port > 65535) 
    {
        std::cerr << RED "Port must be between 1 and 65535.\n" RESET;
        return 1;
    }
    signal(SIGINT, handleSigint);
    // signal(SIGTSTP, handleSigint2);
    try
    {
        Server server(port, argv[2]);
        server.run();
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << RED "💥 Critical error: " RESET << e.what() << std::endl;
        return 1;
    }
    return 0;
}
