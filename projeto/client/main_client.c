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

    return true;
}



int main(int argc, char** argv) {

    char command[SIZE], plid[7], gid[3], ip_address[SIZE], port[6];
    char firstFlag[3], secondFlag[3];
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
    if (!checkArguments(ip_address, port, argc, argv));


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