#include "server.h"
#include "../common.h"

int tcpRead(int fd, char* message, ssize_t size) {
    ssize_t nleft = size, n;
    char* ptr = message;

    while (nleft > 0) {
        n = read(fd, ptr, nleft);
        if (n == -1) {
            puts("ERR Read TCP");
            return -1;
        }
        else if (n == 0) {
            break;
        } 
        nleft -= n;
        ptr += n;
    }
    return 1;
}

int tcpSend(int fd, char* message, ssize_t size) {
    ssize_t nleft = size, n;
    char* ptr = message;

    while (nleft > 0) {
        n = write(fd, ptr, nleft);
        if (n <= 0) {
            puts("ERR Send TCP");
            return -1;
        }
        nleft -= n;
        ptr += n;
    }
    return 1;
}

bool readStr(char* str, int fd) {
    int len = strlen(str);
    char recv[len+1];
    bzero(recv, len+1);
    if (tcpRead(fd, recv, len) == -1 || strcmp(str, recv)) {
        return false;
    }
    return true;
}

bool hint(int fd, bool verbose) {
    printf("tcp here\n");
    char plid[7];
    char playerState[50], imagePath[50];
    bzero(plid, 7);
    bzero(playerState, 50);
    bzero(imagePath, 50);

    if (tcpRead(fd, plid, 6) == -1) {
        return true;
    }

    if (verbose) {
        printf("PLID: %s\n", plid);
    }
    
    sprintf(playerState, "Server/GAMES/GAME_%s.txt", plid);
    if (!readStr("\n", fd)) return false;

    if (access(playerState, F_OK) == -1) {
        tcpSend(fd, "RLG NOK\n", 8);
        return true;
    }
    printf("tcp here\n");
    FILE* player = fopen(playerState, "rb");
    char imageName[50];
    bzero(imageName, 50);
    char holder;
    printf("tcp here\n");
    int i=0;
    while((holder = fgetc(player)) != EOF) {
        if (holder == '\n') break;
        if (holder == ' ') {
            holder = fgetc(player);
            bzero(imageName, 50);
            i=0;
        }
        imageName[i] = holder;
        i++;

    }
    fclose(player);

    //sprintf(imagePath, "Server/%s.jpg", imageName);
    //sprintf(imagePath, "%s", "Server/banana.jpg");

    FILE* image = fopen("Server/banana.jpg", "r");

    fseek(image, 0L, SEEK_END);
  
    // tamanho do ficheiro
    long int fileSize = ftell(image);
    printf("size: %ld\n", fileSize);
    

    char data[fileSize];
    
    fread(data, fileSize, 1, image);
    printf("data: %s", data);
    fclose(image);

    char message[fileSize+12+sizeof("banana.jpg")+sizeof(fileSize)];
    sprintf(message, "RHL OK %s %ld %s\n", "banana.jpg", fileSize, data);
    printf("buffer: %s\n", message);

    if(tcpSend(fd, message, strlen(message)) == -1) {
        return true;
    }
    return true;
}