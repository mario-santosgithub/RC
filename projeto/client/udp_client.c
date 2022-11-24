#include "client.h" 
#include "../common.h"

void test(char* uid, struct addrinfo *res, int fd) {

    char message[20], buffer[BUFFER_SIZE];

    if(udp_send_and_receive(fd, res, message, buffer, BUFFER_SIZE) == -1) return;
}