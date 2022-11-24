#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ctype.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/stat.h>




int udpReceive(int udp_socket, char* message);
int socketBind(int socktype, char* port, struct addrinfo** res);

/* -------------------- server_main -------------------- */

int main(int argc, char** argv);

#endif