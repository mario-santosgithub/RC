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
            else if(holder == EOF) {break;}
        }
        newWord = false;
        printf("here");
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


bool play(int udpSocket, char* plid, char* letter, char* trial, bool verbose) {
    if (verbose)
        printf("PLID: %s\n", plid);

    // Check if Plid only has digits
    if (strlen(plid) == 6) {
        while (*plid) {
            if (*plid < '0' || *plid > '9') { 
                udpSend(udpSocket, "RLG ERR\n", verbose);
                return true;    
            }
            ++plid;
        }
        plid = plid - 6;
    }
    else {
        udpSend(udpSocket, "RLG ERR\n", verbose);
        return true;
    }

    char path[30];
    sprintf(path, "Server/GAMES/GAME_%s.txt", plid);
    int trialInt = atoi(trial);

    // verificar se está em jogo
    if (access(path, F_OK) != 0) {
        udpSend(udpSocket, "RLG ERR\n", verbose);
        return true;
    }

    char line[10];
    bzero(line, 10);
    sprintf(line, "\nT %s", letter);

    FILE* playerState = fopen(path, "r");

    int indexes[50];
    char word[50];
    char holder, temp;
    temp = letter[0];
    printf("t: %c\n", temp);

    int i=0, j=0, wordLen=0;
    while((holder = fgetc(playerState)) != EOF) {
        if (holder == ' ') break;
        if (temp == holder) {
            indexes[j] = i+1;
            j++;
        }
        word[i] = holder;
        i++;
        wordLen++;
    }
    fclose(playerState);

    playerState = fopen(path, "r");
    while((holder = fgetc(playerState)) != EOF) {
        if (holder == 'T') {
            holder = fgetc(playerState);
            holder = fgetc(playerState);

            if (holder == temp) {
                udpSend(udpSocket, "RLG DUP\n", verbose);
                fclose(playerState);
                return true;
            }
        }
    }
    fclose(playerState);
    wordLen--;
    int currentErrors = 0, correctLetters = 0;

    if (j == 0) {

        playerState = fopen(path, "r");
        // verificar erros
        printf("word: %s\n", word);
        while((holder = fgetc(playerState)) != EOF) {
            if (holder == 'T') {
                holder = fgetc(playerState); // ' '
                holder = fgetc(playerState);
                if (holder == temp) {
                        
                    udpSend(udpSocket, "RLG DUP\n", verbose);
                    return true;
                
                }
                printf("h: %c\n", holder);
                bool inWord=false;
                for (int k=0; k<wordLen; k++) {
                    if (holder == word[k]) {
                        inWord=true;
                        correctLetters++;
                    };
                }
                if (inWord == false) {
                    currentErrors++;
                }
            }
        }

        int errors;
        if (wordLen <= 6) { errors = 7; }
        else if (wordLen > 6 && wordLen < 11) { errors = 8; }
        else if (wordLen >= 11) { errors = 9; }

        if (currentErrors >= errors) {
            udpSend(udpSocket, "RLG OVR\n", verbose);

            char newPath[50];
            char newFileName[50];
            sprintf(newPath, "Server/GAMES/%s", plid);
            if (access(newPath, F_OK) != 0) {
                mkdir(newPath, 0700);
            }

            time_t t = time(NULL);
            struct tm tm = *localtime(&t);

            sprintf(newPath, "Server/GAMES/%s/%d%d%d_%d%d%d_F.txt", plid, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

            playerState = fopen(path, "r");

            rename(path, newPath);

        
            fclose(playerState);
            return true;
        }

        // check errors
        fclose(playerState);
        udpSend(udpSocket, "RLG NOK\n", verbose);
        printf("c: %d\n", currentErrors);
        playerState = fopen(path, "a");
        fprintf(playerState, "%s",line);
        fclose(playerState);

        return true;
    }

    // falta verificar o trial
    
    // verificar se é uma letra duplicada
    i=0;
    while((holder = fgetc(playerState)) != EOF) {
        if (temp == holder) {
            indexes[j] = i+1;
            j++;
        }
        i++;
    }

    fclose(playerState);

    char message[BUFF_SIZE];
    // check if its over with a win


    playerState = fopen(path, "a");


    fprintf(playerState, "%s",line);
    fclose(playerState);

    playerState = fopen(path, "r"); // demasiadas igualdades no if, talvez arranjar mais alguma verificação
    int counter = 0;
    bool line1 = true;
    while ((holder = fgetc(playerState)) != EOF) {
        for(int k=0; k<wordLen; k++) {
            if(holder == word[k] && line1 == false) {
                printf("h: %c\n", holder);
                printf("w: %c\n", word[k]);
                counter++;
            }
        }
        if (holder == '\n') { line1 = false; }
    }
    fclose(playerState);

    if (counter == wordLen) {
        sprintf(message, "RLG WIN %s", trial);
        udpSend(udpSocket, message, verbose);

        char newPath[50];
        char newFileName[50];
        sprintf(newPath, "Server/GAMES/%s", plid);
        if (access(newPath, F_OK) != 0) {
            mkdir(newPath, 0700);
        }

        time_t t = time(NULL);
        struct tm tm = *localtime(&t);

        sprintf(newPath, "Server/GAMES/%s/%d%d%d_%d%d%d_W.txt", plid, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

        playerState = fopen(path, "r");

        rename(path, newPath);

    
        fclose(playerState);
        return true;
    }
    printf("counter: %d\n", counter);

    sprintf(message, "RLG OK %d %d", trialInt, j);
    i=0;
    while (i<j) {
        char temp2[5];
        sprintf(temp2, " %d",indexes[i]);
        strcat(message, temp2);
        i++;
    }

    strcat(message, "\n\0");
    printf("m: %s\n", message);
    
    udpSend(udpSocket, message, verbose);
    
    
}

// se precisares de info para ler o ficheiro tens vários exemplos em cima
bool guess(int udpSocket, char* plid, char* word, char* trial, bool verbose) {
    // Check if Plid only has digits
    if (strlen(plid) == 6) {
        while (*plid) {
            if (*plid < '0' || *plid > '9') { 
                udpSend(udpSocket, "RLG ERR\n", verbose);
                return true;    
            }
            ++plid;
        }
        plid = plid - 6;
    }
    else {
        udpSend(udpSocket, "RLG ERR\n", verbose);
        return true;
    }
    char path[30];
    sprintf(path, "Server/GAMES/GAME_%s.txt", plid);
    int checkTrial = 1;
    int trialInt = atoi(trial);

    if (access(path, F_OK) != 0) {
        udpSend(udpSocket, "RLG ERR\n", verbose);
    }

    char line[10];
    bzero(line, 10);

    char wordR[30];
    char currChar;
    int i = 0;
    char message[BUFF_SIZE];
    FILE* playerState = fopen(path, "r");

    if (fgetc(playerState) != ' ')
    {
        currChar = fgetc(playerState);
        wordR[i] = currChar;
        i++;
    } 
    if (strcmp(word,wordR) == 0){
        sprintf(message, "RWG WIN %d\n", trialInt);
    }
    else {
        sprintf(message, "RWG NOK %d\n", trialInt);
    }
    udpSend(udpSocket, message, verbose);

}

bool exitUDP(int udpSocket, char* plid, bool verbose) {

    quitUDP(udpSocket, plid, verbose);
    return true;

}

bool quitUDP(int udpSocket, char* plid, bool verbose) {
    if (strlen(plid) == 6) {
        while (*plid) {
            if (*plid < '0' || *plid > '9') { 
                udpSend(udpSocket, "RLG ERR\n", verbose);
                return true;    
            }
            ++plid;
        }
        plid = plid - 6;
    }
    else {
        udpSend(udpSocket, "RLG ERR\n", verbose);
        return true;
    }

    
    char path[30];
    sprintf(path, "Server/GAMES/GAME_%s.txt", plid);

    if (access(path, F_OK) != 0) {
        return true;
    }

    int k = remove(path);
    printf("%d\n", k);
    return true;
}