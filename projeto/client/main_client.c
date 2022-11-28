#include "client.h"
#include "../common.h"
#include <sys/types.h>
#include <sys/socket.h>

int turn;

int create_socket(struct addrinfo **res, int socktype, char* ip_address, char* port) {
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


void commandExe(int udp_socket, struct addrinfo *res, char* ip_address, char* port, char* command, char* plid, char* groupId) {

    char name[11]; // para o comando scoreboard, de 10 chars + '\0'
    bzero(name, 11);
    sscanf(command, "%s ", name);

    command += strlen(name);
    if (strcmp(name, "start") == 0 || strcmp(name, "sg") == 0 )  {
        sscanf(command, "%s", plid);

        if (strlen(plid) == 6) {
            while (*plid) {
                if (*plid < '0' || *plid > '9') { 
                    printf(ERR_MSG);
                    return;    
                }
                ++plid;
            }
            plid = plid - 6;
            start(udp_socket, plid, res);
        }
        else {
            printf(ERR_MSG);
            return;
        }
    }

    else if (strcmp(name, "play") == 0 || strcmp(name, "pl") == 0 )  {
        printf("here\n");
        char letter[2]; 
        sscanf(command, "%s", letter);
        printf("%s\n", letter);

        if (strlen(letter) == 1) {
            if (*letter < 'A' || ('Z' < *letter && *letter < 'a') || 'z' < *letter) { 
                
                printf(ERR_MSG);
                return;    
            }

            if (*letter >= 'a' && *letter <= 'z') {
                *letter -= 32;
            }
            
            play(udp_socket, plid, letter, turn, res);
        }
        else {
            printf(ERR_MSG);
            return;
        }
        
    }

    else if (strcmp(name, "guess") == 0 || strcmp(name, "gw") == 0 )  {
        
    }

    else if (strcmp(name, "scoreboard") == 0 || strcmp(name, "sb") == 0 )  {
        
    }

    else if (strcmp(name, "hint") == 0 || strcmp(name, "h") == 0 )  {
        
    }

    else if (strcmp(name, "state") == 0 || strcmp(name, "st") == 0 )  {
       
    }

    else if (strcmp(name, "quit") == 0 )  {
        
    }

    else if (strcmp(name, "exit") == 0 )  {
        close(udp_socket);
        freeaddrinfo(res);
        exit(EXIT_SUCCESS);
        return;
    }

    else if (strcmp(name, "kill") == 0 || strcmp(name, "k") == 0 )  {
        sscanf(command, "%s", plid);

        if (strlen(plid) == 6) {
            while (*plid) {
                if (*plid < '0' || *plid > '9') { 
                    printf(ERR_MSG);
                    return;    
                }
                ++plid;
            }
            plid = plid - 6;
            printf("aqui");
            kill(udp_socket, plid, res);
        }
        else {
            printf(ERR_MSG);
            return;
        }
    }
    
    else {
        printf(ERR_MSG);
        return;
    }
    bzero(command, 512);
}


void displayGame(char* buffer) {
    // max buffer size is 128
    char val[BUFFER_SIZE], arg1[31];
    char output[BUFFER_SIZE];
    int n;

    sscanf(buffer, "%s %s", val, arg1);

    if (strcmp(val, "RSG") == 0 && strcmp(arg1, "OK") == 0) {
        buffer += strlen(val) + strlen(arg1) + 2;
        sscanf(buffer, "%s %s", val, arg1); // number of leters
        
        n = atoi(val);
        sprintf(output, "New game started (max %s errors):", arg1);

        for (int i=0; i<n; i++) {
            strcat(output, " _");
        }
    }
    else if (strcmp(val, "RLG") == 0 && strcmp(arg1, "OK") == 0) {
        int turnCheck, n, m;

        char temp[2];

        buffer += strlen(val) + strlen(arg1) + 2;
        sscanf(buffer, "%s %s", temp, val);
        turnCheck = atoi(temp);
        n = atoi(val);
        buffer += strlen(val) + strlen(arg1) + 2;
        
        int positions[n];

        for (int i=0; i< n; i++) {
            sscanf(buffer, "%s", val);
            buffer += strlen(val);

            m = atoi(val);

            positions[i] = m;
        }
        // FICÁMOS AQUI |||||||||||||||||||||||
        
    }

    puts(output);

}



int main(int argc, char** argv) {

    char command[SIZE], plid[7], groupId[3], ip_address[SIZE], port[6];
    char firstFlag[3], secondFlag[3];
    turn = 1;

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
        
        commandExe(udp_socket, res, ip_address, port, command, plid, groupId);
    }

    close(udp_socket);
    freeaddrinfo(res);
    exit(EXIT_SUCCESS);
    return 0;
}
