#include "client.h"

// Client TCP global variable
int tcpSocket;

/***
 * @brief Function responsible for connecting the TCP
 * 
 * @param ip_address string with the ip
 * @param port string with port 
 * @param fd identifier
 * @param res addrinfo struct
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

/***
 * @brief Function responsible for sending TCP messages
 * 
 * @param message string with the message to send
 * @param size size of the messagee
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

/***
 * @brief Function responsible for reading TCP messages
 * 
 * @param buffer string with the message to send
 * @param size size of the messagee
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


/***
 * @brief Function responsible for showing the scoreboard
 * 
 * @param ip_address string with the ip
 * @param port string with port 
 * @param plid string with the player ID
 * @param res addrinfo struct
*/
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
 

    char status[4];
    bzero(status, 4);

    nread = tcpRead(status, 3);
    if (nread == -1) return;


    char endOfMsg[2];
    bzero(endOfMsg, 2);


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


        char filePath[50];
        sprintf(filePath, "PLAYERFILES/%s", fileName);
        FILE* fp = fopen(filePath, "wb");

        if (!fp) {
            close(tcpSocket);
            puts("Erro ao guardar o ficheiro");
            return;
        }

        char size[3];
        nread = tcpRead(size, 3);
        int sizeInt = atoi(size);

        char score[5], playerID[8], word[MAX_WORD_LEN], suc[3], plays[3];
        char line[sizeInt];
        bzero(line, sizeInt);

        nread = tcpRead(line, sizeInt-1);

        
        
        fprintf(fp, "%s",line);
        printf("%s", line);
        fclose(fp);
    }
}

/***
 * @brief Function responsible for showing the hint
 * 
 * @param ip_address string with the ip
 * @param port string with port 
 * @param plid string with the player ID
 * @param res addrinfo struct
*/
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

        char filePath[50];
        sprintf(filePath, "PLAYERFILES/%s", fileName);
        FILE* image = fopen(filePath, "w");
        
        fwrite(data, 1, sizeof(data), image);
        fclose(image);

    }

    close(tcpSocket);
    return;
}

/***
 * @brief Function responsible for showing the state
 * 
 * @param ip_address string with the ip
 * @param port string with port 
 * @param plid string with the player ID
 * @param res addrinfo struct
*/
void state(char* ip_address, char* port, char* plid, struct addrinfo *res) {

    char message[15];
    bzero(message, 15);
    sprintf(message,"STA %s\n",plid);
    if (tcpConnect(ip_address, port, &tcpSocket, res) == -1 || tcpSend(message, strlen(message)) == -1)
        return;

    char result[5];
    bzero(result, 5);

    char status[4];
    bzero(status, 4);


    ssize_t nread = tcpRead(result, 4);
    nread = tcpRead(status, 3);

    if (!strcmp(status, "ACT")) { 
        char aux[2];
        nread = tcpRead(aux, 1);

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

        char size[5];

        index = 0;
        while(true) {
            //printf("fleName: %s\n", fileName);
            nread = tcpRead(size+index, 1);
            if (nread == -1) return;

            if (size[index] == ' ') {
                size[index] = '\0';
                break;
            }

            if (index == 6) {
                close(tcpSocket);
                puts("Nome muito comprido?");
                return;
            }
            index++;
        }

        int sizeInt = atoi(size);

        char data[sizeInt];
        bzero(data, sizeInt+1);
        
        char filePath[50];
        sprintf(filePath, "PLAYERFILES/%s", fileName);
        FILE* state = fopen(filePath, "w");
        
        nread = tcpRead(data, sizeInt);
        printf("%s", data);
        fwrite(data, 1, sizeof(data), state);
        fclose(state);

    }
}

