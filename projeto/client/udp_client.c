#include "client.h" 
#include "../common.h"


int udpTransmission(int fd, struct addrinfo *res, char* message, char* buffer, int size) {

    ssize_t nbytes = sendto(fd, message, strlen(message), 0, res->ai_addr, res->ai_addrlen);
    
    if (nbytes == -1) {
        printf(ERR_MSG);
        return -1;
    } 

    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    
    while(true) {
        nbytes = recvfrom(fd, buffer, size, 0, (struct sockaddr*)&addr, &addrlen);

        if (nbytes == -1) {
            printf("Internal Server Error: Try again\n");
            return -1;
        }
        else { return nbytes; }
    }
}


void start(int fd, char* plid, struct addrinfo *res) {

    char message[20], buffer[BUF_SIZE];

    sprintf(message, "SNG %s\n", plid);
    
    
    if(udpTransmission(fd, res, message, buffer, BUFFER_SIZE) == -1 ){
        printf(ERR_MSG);
        return;
    };

    displayGame(buffer, NULL);
    return;
}

void play(int fd, char* plid, char* letter, int turn, struct addrinfo *res) {
    
    char message[20], buffer[BUF_SIZE];

    sprintf(message, "PLG %s %s %d\n", plid, letter, turn);
    //sprintf(message, "KILLGAME 099275\n");




    if(udpTransmission(fd, res, message, buffer, BUFFER_SIZE) == -1 ){
        printf(ERR_MSG);
        return;
    };


    displayGame(buffer, letter);
}

void kill(int fd, char* plid, struct addrinfo *res) {

    char message[20], buffer[BUF_SIZE];

    sprintf(message, "KILLGAME %s\n", plid);
    
    
    if(udpTransmission(fd, res, message, buffer, BUFFER_SIZE) == -1 ){
        printf(ERR_MSG);
        return;
    };
    return;
}