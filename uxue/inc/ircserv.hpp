#ifndef IRCSERV_H
#define IRCSERV_H

#define DEF_COLOR "\033[0;39m"
#define GRAY "\033[0;90m"
#define RED "\033[0;31m"
#define GREEN "\033[0;92m"
#define YELLOW "\033[0;93m"
#define BLUE "\033[0;94m"
#define MAGENTA "\033[0;95m"
#define CYAN "\033[0;96m"
#define WHITE "\033[0;97m"
#define RESET_COLOR "\033[0m"
#define AQUAMARINE "\033[0;96m"

#include <iostream> //std::cout, std::cerr
#include <cstring>  //memset
#include <cstdlib>  //exit()
#include <unistd.h> //close()
#include <sys/types.h>
#include <sys/socket.h> //socket(), bind(), listen(), accept()
#include <netinet/in.h> // For sockaddr_in structure
#include <arpa/inet.h>  // For inet_ntoa
#include <stdio.h>
#include <stdlib.h> //EXIT_FAILURE

//main.cpp
int check_exec_input(int argc, char **argv);
int get_socket();

#endif