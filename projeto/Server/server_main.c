#include "server.h"
#include "../common.h"


struct sockaddr_in clientAddr;
__socklen_t addrlen;



bool checkPort(char* port) {
    while (*port) {
        if (*port < '0' || *port > '9') { return false; }
        ++port;
    }
}

bool executeUDP(int udpSocket, char* message, bool verbose) {
    
    char opCode[4], arg1[SIZE], arg2[SIZE], arg3[SIZE];
    char delim1, delim2, delim3, delim4;
    bzero(opCode, 4);
    bzero(arg1, SIZE);
    bzero(arg2, SIZE);
    bzero(arg3, SIZE);
    sscanf(message, "%[^ \n]", opCode);
    printf("here\n");
    message += strlen(opCode);
    if (!strcmp(opCode, "SNG")){

        //Player request to start a game
        sscanf(message, "%c%[^ \n]%c", &delim1, arg1, &delim2);

        printf("arg1: %s\n", arg1);
        return delim1 == ' ' && delim2 == '\n' && start(udpSocket, arg1, verbose); 
            

    }
}

int udpSend(int udpSocket, char* message, bool verbose) {

    if (verbose) { printf("Message Sent: %s", message); }

    addrlen = sizeof(clientAddr);
    ssize_t n = sendto(udpSocket, message, strlen(message), 0, (struct sockaddr*)&clientAddr, addrlen);
    
    if (n == -1) {
        puts("SEND_ERR");
        return -1;
    }
    return n;
}

int udpReceive(int udpSocket, char* message) {
    addrlen = sizeof(clientAddr);
    ssize_t n = recvfrom(udpSocket, message, 128, 0, (struct sockaddr*)&clientAddr, &addrlen);
    if (n == -1) {
        puts("RECEIVE ERROR");
        return -1;
    }

    return n;

}


int socket_bind(int socktype, char* port, struct addrinfo** res){
    int fd = socket(AF_INET, socktype, 0);

    if(fd == -1) {
        puts("Erro ao criar a socket");
        close(fd);
        exit(1);
    }

    struct addrinfo hints;
    bzero(&hints, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = socktype;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, port, &hints, res) != 0) {
        puts("ADDR_FAIL");
        close(fd);
        exit(1);
    }

    if (bind(fd, (*res)->ai_addr, (*res)->ai_addrlen) == -1) {
        puts("BIND_FAIL");
        close(fd);
        exit(1); 
    }

    return fd;
}

int main(int argc, char** argv) {

    bool verbose = false;
    char port[6], wordFile[50];
    
    
     // GS  wordFile -p GSport  -v ----
    strcpy(wordFile, argv[1]);
    if (argc == 2) {
        strcpy(port, GROUP_PORT);
    }
    else if (argc == 3) { // only verbose
        if (!strcmp(argv[2], "-v")) {
            verbose = true;
            strcpy(port, GROUP_PORT);
        }
        else {
            puts("Problem with input");
            return 0;
        }
    }

    else if (argc == 4) { // only port
        if (!strcmp(argv[2], "-p")) {
            strcpy(port, argv[3]);
            if(checkPort(port)) return 0;
        }
        else {
            puts("Problem with input");
            return 0;
        }
    }

    else if (argc == 5) { // both
        if (!strcmp(argv[2], "-v") || !strcmp(argv[4], "-v")) {
            verbose = true;
        }
        else {
            puts("Problem with input");
            return 0;
        }

        if (!strcmp(argv[2], "-p")) {
            strcpy(port, argv[3]);
        }
        else if (!strcmp(argv[3], "-p")) {
            strcpy(port, argv[4]);
        }
        else {
            puts("Problem with input");
            return 0;
            if(!checkPort(port)) return 0;
        }  
    }
    else { 
        puts("Problem with input");
        if(!checkPort(port)) return 0;
    }

    // Create a folder for the files that are downloaded using the retrieve command
    char path[70];
    sprintf(path, "SERVERFILES/%s", wordFile);
    FILE *file = fopen(path, "r");
    
    if (!file) {
            puts("Erro ao abrir");
            return 0;
        }

    struct addrinfo *res;
    int udpSocket = socket_bind(SOCK_DGRAM, port, &res);
    // int tcpSocket = socket_bind(SOCK_STREAM, port, &res);

    char message[BUFF_SIZE];


    while(true) {   

        // UDP
        bzero(message, sizeof(message));
        udpReceive(udpSocket, message);
        printf("message: %s\n", message);

        if (!executeUDP(udpSocket, message, verbose)) {
            printf("ERR\n");
        }
        else {
            printf("executeUDP true\n");
        }

    }


    close(udpSocket);
    //close(tcpSocket);
    freeaddrinfo(res);
    return 0;
}
