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


#define GROUP_PORT "58063"
#define BUF_SIZE 128
#define MAX_WORD_LEN 50
#define SIZE 512
#define BUFFER_SIZE 128





/* UDP functions */
int udpTransmission(int fd, struct addrinfo *res, char* message, char* buffer, int size);
void start(int fd, char* plid, struct addrinfo *res);
int play(int fd, char* plid, char* letter, int turn, struct addrinfo *res);
void guess(int fd, char* plid, char* word, int turn, struct addrinfo *res);
void quit(int fd, char* plid, struct addrinfo *res);
void kill(int fd, char* plid, struct addrinfo *res);

/* TCP functions */
void scoreboard(char* ip_address, char* port, char* plid, struct addrinfo *res);
void hint(char* ip_address, char* port, char* plid, struct addrinfo *res);
void state(char* ip_address, char* port, char* plid, struct addrinfo *res);

/* Main functions */
int create_socket(struct addrinfo **res, int socktype, char* ip_address, char* port);
bool checkArguments(char* ip_address, char* port, int argc, char** argv);
bool checkFlags(int argc, char** argv);
void displayGame(char* buffer, char* letter);
void commandExe(int udp_socket, struct addrinfo *res, char* ip_address, char* port, char* command, char* plid, char* groupId);



#endif