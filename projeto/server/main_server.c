#include "server.h"
#include "../common.h"

struct sockaddr_in clientAddr;
socklen_t addrlen;


/**
 * @brief Receive the message from the client
 * 
 * @param udpSocket
 * @param message 
 * @return n bytes received 
 */
int udpReceive(int udpSocket, char* message) {

    addrlen = sizeof(clientAddr);
    ssize_t n = recvfrom(udpSocket, message, 128, 0, (struct sockaddr*)&clientAddr, &addrlen);
    if (n == -1) { return -1; } // error

    printf("message: %s\n", message);
    return n;
}

/**
 * @brief Bind the socket
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int socketBind(int sockType, char* port, struct addrinfo** res) {

    int fd = socket(AF_INET,sockType,0);
    if (fd == -1) { exit(1); } // error

    struct addrinfo hints;
    hints.ai_family = AF_INET;
    hints.ai_socktype = sockType;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, port, &hints, res) != 0) { exit(1); } // error
    
    if (bind(fd, (*res)->ai_addr, (*res)->ai_addrlen) == -1) { exit(1); } // error

    return fd;
}


int main(int argc, char** argv) {

    char port[6];
    strcpy(port, "58026");
    // Create the bind for UDP and TCP
    struct addrinfo *res;
    int udpSocket = socketBind(SOCK_DGRAM, port, &res);
    int tcpSocket = socketBind(SOCK_STREAM, port, &res);
    
    char message[128];
    fd_set rset;
    int conn_fd;

    while (true) {
        FD_SET(udpSocket, &rset);

        if (FD_ISSET(udpSocket, &rset)) {
            udpReceive(udpSocket, message);
        }
    }

    return 0;
}
