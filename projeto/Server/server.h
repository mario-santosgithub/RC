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

#define BUFF_SIZE 128
#define SIZE 512

/* UDP functions*/
bool start(int udpSocket, char* plid, char* fileName, bool verbose);
bool play(int udpSocket, char* plid, char* letter, char* trial, bool verbose);
bool guess(int udpSocket, char* plid, char* word, char* trial, bool verbose);
bool exitUDP(int udpSocket, char* plid, bool verbose);
bool quitUDP(int udpSocket, char* plid, bool verbose);

/* TCP functions */
int tcpRead(int fd, char* message, ssize_t size);
int tcpSend(int fd, char* message, ssize_t size);
bool hint(int fd, bool verbose);

/* Main functions*/
int udpSend(int udpSocket, char* message, bool verbose);
int main(int argc, char** argv);
bool checkPort(char* port);

#endif