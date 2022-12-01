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
    
    char message[5], aux[2];
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

    if (strcmp(status, "OK ") == 0) {printf("ok\n");}

    if (strcmp(status, "OK ") == 0) {
        char fileName[30];
        bzero(fileName, 30);
        int index = 0;

        while(true) {
            //printf("fleName: %s\n", fileName);
            nread = tcpRead(fileName+index, 1);
            if (nread == -1) return;

            if (fileName[index] == ' ') {
                fileName[index] = '\0';
                break;
            }

            if (index == 30) {
                close(tcpSocket);
                puts("Nome muito comprido?");
                return;
            }
            index++;
        }
        char fileSize[4];
        bzero(fileSize, 4);

        nread = tcpRead(fileSize, 3);
        if (nread == -1) return;


        char filePath[40];
        sprintf(filePath, "FILES/%s", fileName);
        FILE* fp = fopen(filePath, "wb");

        if (!fp) {
            close(tcpSocket);
            puts("Erro ao guardar o ficheiro");
            return;
        }


        char score[5], playerID[8], word[MAX_WORD_LEN], suc[3], plays[3];
        char line[128];
        bzero(score, 5);
        bzero(playerID, 7);
        bzero(word, MAX_WORD_LEN);
        bzero(suc, 3);
        bzero(plays, 3);
        
        nread = tcpRead(aux, 1);
        for (int k=0; k< 10; k++) {


            nread = tcpRead(score, 4);
            //printf("s: %s\n", score);

            nread = tcpRead(playerID, 7);
            //printf("s: %s\n", playerID);

            int i=0, l=0;
            while(true) {
                nread = tcpRead(word+i, 1);
                if (nread == -1) return;

                if (word[i] == ' '&& l != 0) {
                    word[i] = '\0';
                    break;
                }

                if (word[i] != ' ') {
                    l = 1;
                }

                if (i == 30) {
                    break;
                }
                i++;
            }

            //printf("w: %s\n", word);    

            nread = tcpRead(suc, 2);
            //printf("s: %s\n", suc);

            if (strlen(suc) == 1) {
                nread = tcpRead(aux, 1);
            }

            nread = tcpRead(score, 2);
            //printf("s: %s\n", score);
            sprintf(line, "%d- player %swith %strials for %ld letter word",k+1,playerID,suc,strlen(word));
            printf("%s\n", line);
            nread = tcpRead(aux, 1);
        }
    }
}