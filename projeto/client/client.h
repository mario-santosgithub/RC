#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <ctype.h>
#include <dirent.h>
#include <stdbool.h>

#define ERR_MSG "ERR\n"


#define SIZE 512
#define BUFFER_SIZE 128

int create_socket(struct addrinfo **res, int socktype, char* ip_address, char* port);


/* Main functions */
bool checkArguments(char* ip_address, char* port, int argc, char** argv);



#endif