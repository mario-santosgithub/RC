#include "client.h"
#include "../common.h"

int tcpSocket;

/**
 * @brief Creates a tcp socket and connects it to the server.
 * 
 * @param ip_address the IP address where the packets will be sent.
 * @param port the port used to send the packets to the desired IP address.
 * @param fd the tcp socket used to communicate.
 * @param res information about the address of the service provider.
 * @return the value that indicates success or failure of the connection attempt.
 */
int tcpConnect(char* ip_address, char* port, int* fd, struct addrinfo *res){
    int value;
    tcpSocket = create_socket(&res, SOCK_STREAM, ip_address, port);
    if ((value = connect(tcpSocket, res->ai_addr, res->ai_addrlen)) == -1){
        puts("CONN_ERR");
        return -1;
    }
    return value;
}

/**
 * @brief Send a message to the server with a fixed size in bytes (TCP).
 * 
 * @param message the message sent to the server.
 * @param size the size of the message sent.
 * @return the value that indicates success or failure. 
 */
int tcpSend(char* message, int size){
    ssize_t nleft = size, nwritten;
    char *ptr = message;
    //Caso o servidor não aceite a mensagem completa, manda por packages
    while (nleft > 0){
        nwritten = write(tcpSocket, ptr, nleft);
        if (nwritten <= 0){
            puts("SEND_ERR");
            return -1;
        }
        nleft -= nwritten;
        ptr += nwritten;  
    }
    return 1;
}

/**
 * @brief Read a message from the server with a fixed size in bytes (TCP).
 * 
 * @param buffer the message that was sent by the server.
 * @param size the size of the buffer.
 * @return the value that indicates success or failure. 
 */
int tcpRead(char* buffer, ssize_t size){
    ssize_t nleft = size, nread;
    char *ptr = buffer;
    while (nleft > 0){
        nread = read(tcpSocket, ptr, nleft);
        if (nread == -1){
            close(tcpSocket);
            puts("RECV_ERR");
            return -1;
        }
        else if (nread == 0)
            break;
        nleft -= nread;
        ptr += nread;
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


        char filePath[40];
        sprintf(filePath, "FILES/%s", fileName);
        FILE* fp = fopen(filePath, "wb");

        if (!fp) {
            close(tcpSocket);
            puts("Erro ao guardar o ficheiro");
            return;
        }


        char score[5], playerID[8], word[MAX_WORD_LEN], suc[3], plays[3];
        char line[12800];

        index = 0;
        while(true) {
            //printf("fleName: %s\n", fileName);
            nread = tcpRead(line+index, 1);
            if (nread == -1) return;

            if (line[index] == '\n') {
                fprintf(fp, "%s",line);
            }

            if (line[index] == '\0') {
                fprintf(fp, "%s",line);
                break;
            }

            if (index == 12800) {
                close(tcpSocket);
                puts("Nome muito comprido?");
                return;
            }
            index++;
        }
        
        fprintf(fp, "%s",line);
        
        fclose(fp);
    }
}


void hint(char* ip_address, char* port, char* plid, struct addrinfo *res){
    

    char message[15];
    bzero(message, 15);
    sprintf(message,"GHL %s\n",plid);
    if (tcpConnect(ip_address, port, &tcpSocket, res) == -1 || tcpSend(message, strlen(message)) == -1)
        return;

    char result[5];
    bzero(result, 5);

    ssize_t nread = tcpRead(result, 4);

    char status[4];
    bzero(status, 4);

    nread = tcpRead(status, 3);

    if (!strcmp(status, "OK ")) {

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


        char size[20];
        index = 0;

        while(true) {
            //printf("fleName: %s\n", fileName);
            nread = tcpRead(size+index, 1);
            if (nread == -1) return;

            if (size[index] == ' ') {
                size[index] = '\0';
                break;
            }

            if (index == 20) {
                close(tcpSocket);
                puts("Nome muito comprido?");
                return;
            }
            index++;
        }
        int sizeInt = atoi(size);

        char data[sizeInt];
        nread = tcpRead(data, sizeInt);

        char filePath[40];
        sprintf(filePath, "FILES/%s", fileName);
        FILE* image = fopen(filePath, "w");
        
        fwrite(data, 1, sizeof(data), image);
        fclose(image);

    }

    close(tcpSocket);
    return;
}

