#include "client.h" 
#include "../common.h"


int kappa(int fd, struct addrinfo *res, char* message, char* buffer, int size) {
    printf("entrei\n");

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

    printf("aqui\n");
    return 3;
}


/*int kappa(int fd, struct addrinfo *res, char* message, char* buffer, int size) {
    printf("entrei");
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
}*/


void start(int fd, char* plid, struct addrinfo *res) {

    char message[20], buffer[BUF_SIZE];

    sprintf(message, "SNG %s\n", plid);
    printf("message sent: %s\n",message);
    
    printf("1\n");


    printf("2\n");


    printf("3\n");


    printf("4\n");



    printf("5\n");
    
    int k = kappa(fd, res, message, buffer, BUFFER_SIZE);

    printf("buffer: %s\n", buffer);


}