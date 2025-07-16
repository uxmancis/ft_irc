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

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << RED "Usage: ./ircserv [PORT] [PASSWORD]\n" RESET;
        return 1;
    }
    int port = 0;
    try
    {
        port = std::atoi(argv[1]);
    }
    catch (const std::exception &)
    {
        std::cerr << RED "Invalid port: must be a number between 1 and 65535.\n" RESET;
        return 1;
    }
    if (port <= 0 || port > 65535)
    {
        std::cerr << RED "Port must be between 1 and 65535.\n" RESET;
        return 1;
    }
    struct sigaction sa;
    sa.sa_handler = handleSigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        std::cerr << RED "Failed to set up signal handler\n" RESET;
        return 1;
    }
    try
    {
        Server server(port, argv[2]);
        server.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << RED "💥 Critical error: " RESET << e.what() << std::endl;
        return 1;
    }
    return 0;
}
