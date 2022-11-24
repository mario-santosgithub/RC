#include "client.h"
#include "../common.h"
#include <sys/types.h>
#include <sys/socket.h>


int create_socket(struct addrinfo **res, int socktype, char* ip_address, char* port){
    int sockfd = socket(AF_INET,socktype,0);
    if (sockfd == -1){ exit(1); } // eror
    struct addrinfo hints;
    bzero(&hints, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = socktype;
    if(getaddrinfo(ip_address, port, &hints, res) != 0) { exit(1); } // eror
    return sockfd;
}


bool checkArguments(char* ip_address, char* port, int argc, char** argv){

    return true;
}



int main(int argc, char** argv) {

    char command[SIZE], plid[7], gid[3], ip_address[SIZE], port[6];

    strcpy(ip_address, argv[1]); 
    strcpy(port, argv[2]);
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