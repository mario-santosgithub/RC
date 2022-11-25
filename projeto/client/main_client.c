#include "client.h"
#include "../common.h"
#include <sys/types.h>
#include <sys/socket.h>


int create_socket(struct addrinfo **res, int socktype, char* ip_address, char* port){
    int sockfd = socket(AF_INET,socktype,0);
    if (sockfd == -1){ 
        exit(1); 
        printf(ERR_MSG);    
    } 

    struct addrinfo hints;
    bzero(&hints, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = socktype;
    
    if(getaddrinfo(ip_address, port, &hints, res) != 0) { 
        exit(1); 
        printf(ERR_MSG);    
    }
    return sockfd;
}


bool checkArguments(char* ip_address, char* port, int argc, char** argv){
    if (port == NULL) { return false; }
    int size = strlen(port);
    char temp[size];
    strcpy(temp, port);

    while (*port) {
        if (*port < '0' || *port > '9') { return false; }
        ++port;
    }

    int n = atoi(temp);
    if (n < 1024 || n > 65535) { return false; }

    return true;
}


bool checkFlags(int argc, char** argv) {
    if (strcmp(argv[1], "-n") == 0) {
        if (strcmp(argv[3], "-p") == 0) { return true; }
    }
    if (strcmp(argv[3], "-n") == 0) {
        if (strcmp(argv[1], "-p") == 0) { return true; }
    }

    return false;
}


int main(int argc, char** argv) {

    char command[SIZE], plid[7], gid[3], ip_address[SIZE], port[6];
    char firstFlag[3], secondFlag[3];


    if (argc != 5 || !checkFlags(argc, argv)) {
        printf(ERR_MSG);
        printf("Formatação errada, exemplo de utilização correta: './player -n lima -p 58000'\n");
        return 0;
    }

    strcpy(firstFlag, argv[1]);

    if (strcmp(firstFlag, "-n") == 0) {
        strcpy(ip_address, argv[2]); 
        strcpy(port, argv[4]);
    }
    else {
        strcpy(ip_address, argv[4]); 
        strcpy(port, argv[2]);
    } 

    printf("%s\n",ip_address);
    printf("%s\n", port);
    /* fazer qualquer coisa para verificar os argumentos e dar msgs de erro */
    if (!checkArguments(ip_address, port, argc, argv)) {
        printf(ERR_MSG);
        printf("Formatação errada, exemplo de utilização correta: './player -n lima -p 58000'\n");
        return 0;
    }


    struct addrinfo *res;
    int udp_socket = create_socket(&res, SOCK_DGRAM, ip_address, port);
    


    // Read commands
    while(fgets(command, SIZE, stdin)) {
        
        char name[12];
        if(sscanf(command, "%s ", name) < 1) {
            printf("errado\n");
            return 0;
        }
    }

    close(udp_socket);
    freeaddrinfo(res);
    exit(EXIT_SUCCESS);
    return 0;
}
