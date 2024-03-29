#include "client.h"
#include <sys/types.h>
#include <sys/socket.h>

// Client global variables
int turn;
char word[MAX_WORD_LEN];
int k;

/***
 * @brief Checks the arguments of the input are correct
 * 
 * @param ip_address string with the ip
 * @param port string with port 
 * @param argc number of input arguments
 * @param argv array with the input arguments
 * return false otherwise
*/
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

/***
 * @brief Checks the flags of the input are correct
 * 
 * @param argc number of input arguments
 * @param argv array with the input arguments
 * return false otherwise
*/
bool checkFlags(int argc, char** argv) {
    if (strcmp(argv[1], "-n") == 0) {
        if (strcmp(argv[3], "-p") == 0) { return true; }
    }
    if (strcmp(argv[3], "-n") == 0) {
        if (strcmp(argv[1], "-p") == 0) { return true; }
    }

    return false;
}

/***
 * @brief Creates a socket to comunicate with the client
 * 
 * @param res addrinfo struct
 * @param socktype the type of the socket, UDP or TCP
 * @param ip_address string with the ip
 * @param port string with port 
 * return false otherwise
*/
int create_socket(struct addrinfo **res, int socktype, char* ip_address, char* port){
    int sockfd = socket(AF_INET,socktype,0);
    if (sockfd == -1){
        puts("SOCK_FAIL");
        exit(EXIT_FAILURE);
    }
    struct addrinfo hints;
    bzero(&hints, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = socktype;
    if(getaddrinfo(ip_address, port, &hints, res) != 0){
        puts("ADDR_FAIL");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}


/***
 * @brief Executes the commands from terminal
 * 
 * @param udp_socket identifies the socket
 * @param res addrinfo struct
 * @param ip_address string with the ip
 * @param port string with port 
 * @param command string with the command to execute
 * @param plid string with the player ID
 * @param groupID string with the group id
 * return false otherwise
*/
void commandExe(int udp_socket, struct addrinfo *res, char* ip_address, char* port, char* command, char* plid, char* groupID){
    char name[12]; // The largest command name has 11 characters + '\0'
    char arg1[SIZE];
    char arg2[SIZE];
    char arg3[SIZE];
    char password[SIZE];
    char gid[SIZE];
    char uid[SIZE];
    bzero(name, 12);
    bzero(arg1, SIZE);
    bzero(arg2, SIZE);
    bzero(arg3, SIZE);
    if (sscanf(command, "%s ", name) < 1){ // Check if there's at least a command name
        puts("INVALID_CMD");
        return;
    }
    command += strlen(name) + 1; // The name has already been read!
    if (!strcmp(name, "start") || !strcmp(name, "sg")){
        // Post (TCP): arg1 = text, arg2 = FName (optional)
        // There must be a logged in user and a group selected
        sscanf(command, "%s", plid);
        turn = 0;
        bzero(word, 50);

        if (strlen(plid) == 6) {
            while (*plid) {
                if (*plid < '0' || *plid > '9') { 
                    printf("ERR_MSG: Invalid Player ID\n");
                    return;    
                }
                ++plid;
            }
            plid = plid - 6;
            start(udp_socket, plid, res);
        }
        else {
            printf("ERR_MSG: Invalid Player ID\n");
            return;
        }
        return;
    }
    else if (strcmp(name, "play") == 0 || strcmp(name, "pl") == 0 )  {
        if (strlen(word) == 0) {
            printf("Internal error: No given plid.\n");
            return;
        }
        char letter[2]; 
        bzero(letter, 2);
        sscanf(command, "%s", letter);

        if (strlen(letter) == 1) {
            if (*letter < 'A' || ('Z' < *letter && *letter < 'a') || 'z' < *letter) { 
                
                printf("Not a valid letter\n");
                return;    
            }

            if (*letter >= 'a' && *letter <= 'z') {
                *letter -= 32;
            }
            
            if(play(udp_socket, plid, letter, turn, res) != -1) { turn++; }
            
        }
        else {
            printf("Format error!\n");
            return;
        }
        
    }

    else if (strcmp(name, "guess") == 0 || strcmp(name, "gw") == 0 )  {
        
        char guessWord[MAX_WORD_LEN];
        sscanf(command, "%s", guessWord);

        int size = strlen(guessWord);

        for (int i=0; i<size; i++) {
            if (guessWord[i] <'A' || ('Z' < guessWord[i] && guessWord[i] <'a') || 'z' < guessWord[i]) {
                printf("Invalid Word.\n");
                return;
            }

            if (guessWord[i] >= 'a' && guessWord[i] <= 'z') {
                guessWord[i] -= 32;
            }

        }

        guess(udp_socket, plid, guessWord, turn, res);

    }

    else if (strcmp(name, "scoreboard") == 0 || strcmp(name, "sb") == 0 )  {
        scoreboard(ip_address, port, plid, res);
    }

    else if (!strcmp(name, "hint") || !strcmp(name, "h")){

        hint(ip_address, port, plid, res);
    
    } 

    else if (strcmp(name, "state") == 0 || strcmp(name, "st") == 0 )  {
       state(ip_address, port, plid, res);
    }

    else if (strcmp(name, "quit") == 0 )  {
        sscanf(command, "%s", plid);
        if (strlen(plid) == 6) {
            while (*plid) {
                if (*plid < '0' || *plid > '9') { 
                    printf("ERR_MSG: Invalid Player ID\n");
                    return;    
                }
                ++plid;
            }
            plid = plid - 6;
            quit(udp_socket, plid, res);
        }
        else {
            printf("ERR_MSG: Invalid Player ID\n");
            return;
        }
        
    }

    else if (strcmp(name, "exit") == 0 )  {
        quit(udp_socket, plid, res);
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
                    printf("ERR_MSG: Invalid Player ID\n");
                    return;    
                }
                ++plid;
            }
            plid = plid - 6;
            
            kill(udp_socket, plid, res);
        }
        else {
            printf("ERR_MSG: Invalid Player ID\n");
            return;
        }
    }
    else
        puts("INVALID_CMD");
}

/***
 * @brief Displays the game after a turn
 * 
 * @param buffer has the information a turn
 * @param letter has the letter played
 * return false otherwise
*/

void displayGame(char* buffer, char* letter) {
    // max buffer size is 128
    char val[BUFFER_SIZE], arg1[31];
    char output[BUFFER_SIZE];
    int n;
    
    sscanf(buffer, "%s %s", val, arg1);
    buffer += strlen(val) + strlen(arg1) + 2;
    
    if (strcmp(val, "RSG") == 0 && strcmp(arg1, "OK") == 0) {
        
        sscanf(buffer, "%s %s", val, arg1); // number of leters
        
        for (int i=0; i<MAX_WORD_LEN; i++) {
            word[i] = ' ';
        }
        turn = 1;
        n = atoi(val);
        sprintf(output, "New game started (max %s errors): ", arg1);
        k = n;
        for (int i=0; i<n; i++) {
            strcat(output, "_");
            word[i] = '_';
        }

    }
    else if (strcmp(val, "RSG") == 0 && strcmp(arg1, "NOK") == 0) {
        sprintf(output, "Error: This player has already started a game!");

    }    
    else if (strcmp(val, "RSG") == 0 && strcmp(arg1, "ERR") == 0) {
        sprintf(output, "Error: Syntax Error");

    }
    else if (strcmp(val, "RLG") == 0 && strcmp(arg1, "OK") == 0) {
        int turnCheck, n, m;

        char temp[2];
       
     
        sscanf(buffer, "%s %s", temp, val);
        turnCheck = atoi(temp);
        n = atoi(val);
        buffer += strlen(temp) + strlen(val) + 2;
        

        for (int i=0; i< n; i++) {
            sscanf(buffer, "%s", val);
            buffer += strlen(val) + 1;

            m = atoi(val);
            word[m-1] = *letter;
        }

        
        sprintf(output, "Yes, \"%s\" is part of the word: ", letter);
        strcat(output, word);

    }

    else if (strcmp(val, "RLG") == 0 && strcmp(arg1, "NOK") == 0) {
        
        int turnCheck, n, m;

        char temp[2];
        
        
        sprintf(output, "No, \"%s\" is not part of the word: ", letter);
        strcat(output, word);
        
    }

    else if (strcmp(val, "RLG") == 0 && strcmp(arg1, "DUP") == 0) {
        sprintf(output, "You have already played the letter \"%s\"!: ", letter);
        strcat(output, word);
        turn -= 1;
    }

    else if ((strcmp(val, "RLG") == 0 && strcmp(arg1, "WIN") == 0) ||
        (strcmp(val, "RWG") == 0 && strcmp(arg1, "WIN") == 0)) {

        if (strcmp(val, "RLG") == 0) {
            for (int i=0; i<k; i++) {
                if (word[i] == '_') {
                    word[i] = *letter;
                }
            }
            sprintf(output, "WELL DONE! You guessed: %s", word);
        }
        else {
            sprintf(output, "WELL DONE! You guessed: %s", letter);
        }
    }

    else if (strcmp(val, "RLG") == 0 && strcmp(arg1, "OVR") == 0) {

        sprintf(output, "That was not the correct letter! You have no more chances to play. Game over!");
    }

    else if (strcmp(val, "RLG") == 0 && strcmp(arg1, "INV") == 0) {

        sprintf(output, "Internal Error: Problem with the turn count!");
    }

    else if (strcmp(val, "RLG") == 0 && strcmp(arg1, "ERR") == 0) {

        sprintf(output, "The player is not in a game!");
    }

    else if (strcmp(val, "RWG") == 0 && strcmp(arg1, "DUP") == 0) {
        sprintf(output, "You have already played the letter \"%s\"!: ", letter);
        strcat(output, word);
        turn -= 1;
    }
    
    else if (strcmp(val, "RWG")== 0 && strcmp(arg1, "NOK") == 0) {

        sprintf(output, "That was not the correct word!");
        turn += 1;
    }

    else if (strcmp(val, "RWG")== 0 && strcmp(arg1, "OVR") == 0) {

        sprintf(output, "That was not the correct word! You have no more chances to guess. Game over!");
        
    }

    else if (strcmp(val, "RWG")== 0 && strcmp(arg1, "INV") == 0) {

        sprintf(output, "Internal Error: Problem with the turn count!");
        
    }

    else if (strcmp(val, "RWG") == 0 && strcmp(arg1, "ERR") == 0) {

        sprintf(output, "Internal error: PWG sent with syntax error!\n");
    }

    else if (strcmp(val, "RQT") == 0 && strcmp(arg1, "NOK") == 0) {

        sprintf(output, "No ongoing game for this Player IDS");
    }

    else if (strcmp(val, "RQT") == 0 && strcmp(arg1, "ERR") == 0) {

        sprintf(output, "Internal Error with your request");
    }

    else {puts("Error receiving answer");}
    puts(output);

}

/***
 * @brief The main function of the program
 * 
 * @param argc number of the input arguments
 * @param argv array with the input arguments
 * return false otherwise
*/
int main(int argc, char** argv){
    char command[SIZE], plid[7], groupID[3], ip_address[512], port[6];
    char firstFlag[3], secondFlag[3];
    // colocar checks

    if (argc == 1) {
        strcpy(port, GROUP_PORT);
        strcpy(ip_address, "localhost");
    }
    else if (argc == 3) {
        
        if (strcmp(argv[1], "-n") == 0) {
            strcpy(ip_address, argv[2]);
            strcpy(port, GROUP_PORT);
        }
        else if (strcmp(argv[1], "-p") == 0) {
            strcpy(port, argv[2]);
            strcpy(ip_address, "localhost");
        }
    }
    else {
        strcpy(firstFlag, argv[1]);
        if (argc != 5 || !checkFlags(argc, argv)) {
            printf("Formatação errada, exemplo de utilização correta: './player -n lima -p 58000'\n");
            return 0;
        }

        if (strcmp(firstFlag, "-n") == 0) {
            strcpy(ip_address, argv[2]); 
            strcpy(port, argv[4]);
        }
        if (strcmp(firstFlag, "-p") == 0){
            strcpy(ip_address, argv[4]); 
            strcpy(port, argv[2]);
        } 

        /* fazer qualquer coisa para verificar os argumentos e dar msgs de erro */
        if (!checkArguments(ip_address, port, argc, argv)) {
            printf("Formatação errada, exemplo de utilização correta: './player -n lima -p 58000'\n");
            return 0;
        }
    }

    struct addrinfo *res;
    int udp_socket = create_socket(&res, SOCK_DGRAM, ip_address, port);
    bzero(plid, 7);
    bzero(groupID, 3);

    // Create a folder for the files that are downloaded using the retrieve command
    if (mkdir("PLAYERFILES", 0700) == -1 && access("PLAYERFILES", F_OK)){
        puts("PLAYER_FAIL");
        exit(EXIT_FAILURE);
    }

    // Read commands
    while(fgets(command, SIZE, stdin)){
        commandExe(udp_socket, res, ip_address, port, command, plid, groupID);
        bzero(command, SIZE);
    }

    close(udp_socket);
    freeaddrinfo(res);
    exit(EXIT_SUCCESS);
}