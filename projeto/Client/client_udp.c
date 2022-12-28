#include "client.h" 


int timerOn(int sd, int time){
    struct timeval tmout;
    memset((char *)&tmout,0,sizeof(tmout)); /* Clear time structure. */
    tmout.tv_sec = time;
    return setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tmout,sizeof(struct timeval));
}


int timerOff(int sd){
    struct timeval tmout;
    memset((char *)&tmout,0,sizeof(tmout)); /* Clear time structure */
    return setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tmout,sizeof(struct timeval));
}

/***
 * @brief Function responsible for sending UDP messages
 *
 * @param fd identifier
 * @param res addrinfo struct
 * @param message string with the message to send
 * @param buffer string with the message to send
 * @param size size of the message
 * return false otherwise
*/

int udpTransmission(int fd, struct addrinfo *res, char* message, char* buffer, int size) {

    ssize_t nbytes = sendto(fd, message, strlen(message), 0, res->ai_addr, res->ai_addrlen);
    
    if (nbytes == -1) {
        printf("Error with communication with UDP\n");
        return -1;
    } 

    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    
    while(true) {
        timerOn(fd, 5);
        nbytes = recvfrom(fd, buffer, size, 0, (struct sockaddr*)&addr, &addrlen);
        timerOff(fd);

        
        if (nbytes == -1) {
            printf("Internal Server Error: Try again\n");
            return -1;
        }
        else { return nbytes; }
    }
}

/***
 * @brief Function responsible for starting the game
 *
 * @param fd identifier
 * @param plid string with the player ID
 * @param res addrinfo struct
 * return false otherwise
*/

void start(int fd, char* plid, struct addrinfo *res) {

    char message[20], buffer[BUF_SIZE];

    sprintf(message, "SNG %s\n", plid);
    
    
    if(udpTransmission(fd, res, message, buffer, BUFFER_SIZE) == -1 ){
        printf("Error with communication with UDP\n");
        return;
    };

    displayGame(buffer, NULL);
    return;
}

/***
 * @brief Function used to play in a turn
 *
 * @param fd identifier
 * @param plid string with the player ID
 * @param letter has the letter played
 * @param turn int of the current turn
 * @param res addrinfo struct
 * return false otherwise
*/

int play(int fd, char* plid, char* letter, int turn, struct addrinfo *res) {
    
    char message[20], buffer[BUF_SIZE];

    sprintf(message, "PLG %s %s %d\n", plid, letter, turn);




    if(udpTransmission(fd, res, message, buffer, BUFFER_SIZE) == -1 ){
        printf("Error with communication with UDP\n");
        return -1;
    };

    displayGame(buffer, letter);


    return 1;
}

/***
 * @brief Function used kill a game
 *
 * @param fd identifier
 * @param plid string with the player ID
 * @param res addrinfo struct
 * return false otherwise
*/

void kill(int fd, char* plid, struct addrinfo *res) {

    char message[20], buffer[BUF_SIZE];

    sprintf(message, "KILLGAME %s\n", plid);
    
    
    if(udpTransmission(fd, res, message, buffer, BUFFER_SIZE) == -1 ){
        printf("Error with communication with UDP\n");
        return;
    };
    return;
}

/***
 * @brief Function used to guess in a turn
 *
 * @param fd identifier
 * @param plid string with the player ID
 * @param word string with the guessed word
 * @param turn int of the current turn
 * @param res addrinfo struct
 * return false otherwise
*/

void guess(int fd, char* plid, char* word, int turn, struct addrinfo *res) {
    char message[20], buffer[BUFFER_SIZE];

    sprintf(message, "PWG %s %s %d\n", plid, word, turn);
    if (strlen(word) == 1) { 
        puts("That is not a word!");
        return;
    }
    if(udpTransmission(fd, res, message, buffer, BUFFER_SIZE) == -1 ) {
        printf("Error with communication with UDP\n");
        return;
    }

    displayGame(buffer, word);
    return;
}

/***
 * @brief Function used quit from the game
 *
 * @param fd identifier
 * @param plid string with the player ID
 * @param res addrinfo struct
 * return false otherwise
*/

void quit(int fd, char* plid, struct addrinfo *res) {
    char message[20], buffer[BUF_SIZE];

    sprintf(message, "QUT %s\n", plid);
    
    
    if(udpTransmission(fd, res, message, buffer, BUFFER_SIZE) == -1 ){
        printf("Error with communication with UDP\n");
        return;
    };

    //displayGame(buffer, NULL);
    return;
}