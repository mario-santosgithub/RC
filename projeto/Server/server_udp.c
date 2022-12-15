#include "server.h"
#include "../common.h"



bool start(int udpSocket, char* plid, char* fileName, bool verbose){
    if (verbose)
        printf("PLID: %s\n", plid);

    // Check if Plid only has digits
    if (strlen(plid) == 6) {
        while (*plid) {
            if (*plid < '0' || *plid > '9') { 
                udpSend(udpSocket, "RSG ERR\n", verbose);
                return true;    
            }
            ++plid;
        }
        plid = plid - 6;
    }
    else {
        udpSend(udpSocket, "RSG ERR\n", verbose);
        return true;
    }

    bool newWord = true;
    // check if the player has a game started
    FILE *state;
    char stateName[50];
    
    sprintf(stateName, "Server/GAMES/GAME_%s.txt", plid);
    

    if (access(stateName, F_OK) == 0) {
        
        state = fopen(stateName, "rb");
        char holder;
        int currentLine = 0;

        while(true) {
            holder=fgetc(state);
            
            if(holder == '\n') { 
                udpSend(udpSocket, "RSG NOK\n", verbose);
                fclose(state);
                return true;
            }
        }
        newWord = false;
        
        fclose(state);
    }

    char word[50], hint[50];
    bzero(word, 50);
    bzero(hint, 50);

    if (newWord == true) {
    
        char path[60];
        sprintf(path, "Server/%s", fileName);

        FILE *words = fopen(path, "rb");

        int wordIndex;
        wordIndex = rand() % 5;

        // get the word
        int currentLine = 0;
        
        fseek(words, 0, SEEK_SET);

        int index=0;
        
        char holder;
        while(currentLine != wordIndex) {
            holder=fgetc(words);
            if(holder == '\n') { currentLine++; }
        }

        index = 0;
        while((holder = fgetc(words)) != EOF && holder != '\n') {
            if (holder == ' ') break;
            word[index] = holder;
            index++;
            
        }

        index = 0;
        while((holder = fgetc(words)) != EOF && holder != '\n') {
            hint[index] = holder;
            index++;
        }
        fclose(words);
    }
    else if (newWord == false) {
        char path[60];
        sprintf(path, "Server/GAMES/GAME_%s.txt", plid);

        FILE *state = fopen(path, "rb");

        // get the word
        int currentLine = 0;
        
        fseek(state, 0, SEEK_SET);

        int index=0;
        char holder;
        
        while((holder = fgetc(state)) != EOF) {
            if (holder == ' ') break;
            word[index] = holder;
            index++;
            
        }

        index = 0;
        while((holder = fgetc(state)) != EOF) {
            hint[index] = holder;
            index++;
        }

        fclose(state);
    }

    char line[150];
    sprintf(line, "%s %s", word, hint);

    if (newWord == true) {
        FILE* fp = fopen(stateName, "wb");
        fprintf(fp, "%s", line);
        fclose(fp);
    }

    long wordLen = strlen(word);
    int errors;
    if (wordLen <= 6) { errors = 7; }
    else if (wordLen > 6 && wordLen < 11) { errors = 8; }
    else if (wordLen >= 11) { errors = 9; }


    char messageSent[50];
    sprintf(messageSent, "RSG OK %ld %d\n", wordLen, errors);
    
    udpSend(udpSocket, messageSent, verbose);
    return true;
}