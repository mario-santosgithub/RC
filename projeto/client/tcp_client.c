#include "client.h" 
#include "../common.h"

int tcpSocket;

int tcpConnect(char* ip_address, char* port, int* fd, struct addrinfo *res) {
    int result;
    tcpSocket = create_socket(&res, SOCK_STREAM, ip_address, port);
    result = connect(tcpSocket, res->ai_addr, res->ai_addrlen);
    
    if (result == -1) {
        puts(ERR_MSG);
        return -1;
    }
    return result;
}

int tcpSend(char* message, int size) {

    ssize_t nleft = size, nwritten;
    char *p = message;

    while (nleft > 0) {
        nwritten = write(tcpSocket, p, nleft);
        if (nwritten <= 0) {
            puts("erro enviar tcp");
            return -1;
        }

        nleft -= nwritten;
        p += nwritten;
    }
    return 1;
}

int tcpRead(char* buffer, ssize_t size) {
    ssize_t nleft = size, nread;
    char *p = buffer;

    while (nleft > 0) {
        nread = read(tcpSocket, p, nleft);
        if (nread == -1) {
            close(tcpSocket);
            puts("erro receber tcp");
            return -1;
        }
        else if (nread == 0) {
            break;
        }
        nleft -= nread;
        p += nread;
    }
    return 1;
}


void scoreboard(char* ip_address, char* port, char* plid, struct addrinfo *res) {
    
    char message[5];
    bzero(message, 5);
    sprintf(message, "GSB\n");

    if (tcpConnect(ip_address, port, &tcpSocket, res) == -1 ||
        tcpSend(message, strlen(message)) == -1) return;

    
    
    char result[5];
    bzero(result, 5);

    ssize_t nread = tcpRead(result,4);
    if (nread == -1) return;
    
    printf("result: %s\n",result);

    char status[4];
    bzero(status, 4);

    nread = tcpRead(status, 3);
    if (nread == -1) return;
    printf("status: %s\n", status);

    char endOfMsg[2];
    bzero(endOfMsg, 2);

    if (strcmp(status, "Ok ") == 0) {
        
    }

/*


    
    char end[2];
    bzero(end, 2);

    if (strcmp(status, "NOK") == 0) {
        printf("here");
        nread = tcpRead(end, 1);
        if (nread == -1) return;
        if (strcmp("\n", end) == 0) {
            puts("fail");
            return;
        }
        close(tcpSocket);
        return;
    }

    if (strcmp(status, "OK ") == 0) {
        printf("here");
        char playerID[20];
        bzero(playerID, 20);

        while(true) {
            nread = tcpRead(playerID, 1);
            if (nread == -1) return;

        }
    }
*/
}