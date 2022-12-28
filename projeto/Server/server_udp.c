#include "server.h"



int wordIndex = 0;


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
            holder = toupper(holder);
            
            if(holder == '\n') { 
                udpSend(udpSocket, "RSG NOK\n", verbose);
                fclose(state);
                return true;
            }
            else if(holder == EOF) {break;}
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

        // get the word
        int currentLine = 0;
        
        fseek(words, 0, SEEK_SET);

        int index=0;
        
        char holder;
        while(currentLine != wordIndex) {
            holder=fgetc(words);
            holder = toupper(holder);
            if(holder == '\n') { currentLine++; }
        }

        index = 0;
        while((holder = fgetc(words)) != EOF && holder != '\n') {
            holder = toupper(holder);
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
            holder = toupper(holder);
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
    if (wordIndex != 25) {
        wordIndex++;
    }
    else {
        wordIndex = 0;
    }
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
    bzero(path, 30);
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
    bzero(word, 50);
    temp = letter[0];

    int i=0, j=0, wordLen=0;
    while((holder = fgetc(playerState)) != EOF) {
        holder = toupper(holder);
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
    bool firstLine = true;
    while((holder = fgetc(playerState)) != EOF) {
        holder = toupper(holder);
        if (holder == '\n') { firstLine == false; }
        if (holder == 'T' && firstLine == false) {
            holder = fgetc(playerState);
            holder = fgetc(playerState);
            holder = toupper(holder);

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
        bool firstLine = true;
        while((holder = fgetc(playerState)) != EOF) {
            holder = toupper(holder);
            if (holder == '\n') { firstLine = false; }
            if (holder == 'T' && firstLine == false) {
                holder = fgetc(playerState); // ' '
                holder = fgetc(playerState);
                holder = toupper(holder);
                if (holder == temp) {
                        
                    udpSend(udpSocket, "RLG DUP\n", verbose);
                    return true;
                
                }
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
        wordLen++;
        if (wordLen <= 6) { errors = 7; }
        else if (wordLen > 6 && wordLen < 11) { errors = 8; }
        else if (wordLen >= 11) { errors = 9; }

        if (currentErrors >= errors) {
            udpSend(udpSocket, "RLG OVR\n", verbose);

            char newPath[50];
            char newFileName[50];
            bzero(newFileName, 50);
            bzero(newPath, 50);
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
        playerState = fopen(path, "a");
        fprintf(playerState, "%s",line);
        fclose(playerState);

        return true;
    }

    // falta verificar o trial
    // verificar se é uma letra duplicada
    i=0;
    while((holder = fgetc(playerState)) != EOF) {
        holder = toupper(holder);
        if (temp == holder) {
            indexes[j] = i+1;
            j++;
        }
        i++;
    }


    char message[BUFF_SIZE];
    bzero(message, BUFF_SIZE);

    playerState = fopen(path, "a"); 

    char lineaux[10];
    strcpy(lineaux, line);

    fprintf(playerState, "%s",line);
    playerState = fopen(path, "r"); 
    int counter = 0;
    bool line1 = true;
    while ((holder = fgetc(playerState)) != EOF) {
        holder = toupper(holder);
        if (holder == '\n') { line1 = false; }
        if (line1 == false && holder == 'T') {
            holder = fgetc(playerState);
            holder = fgetc(playerState);
            holder = toupper(holder);
            for(int k=0; k<wordLen; k++) {
                if(holder == word[k] && line1 == false) {
                    counter++;
                }
            }
        }
    
    }
    fclose(playerState);

    if (counter == wordLen-1) {
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

    sprintf(message, "RLG OK %d %d", trialInt, j);
    i=0;
    while (i<j) {
        char temp2[5];
        sprintf(temp2, " %d",indexes[i]);
        strcat(message, temp2);
        i++;
    }

    playerState = fopen(path, "a");
    fprintf(playerState, "%s", line);
    fclose(playerState);
    strcat(message, "\n\0");
    udpSend(udpSocket, message, verbose);
    
    
}

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

    char line[50];
    bzero(line, 50);
    sprintf(line, "\nG %s", word);

    FILE* playerState = fopen(path, "r");

    int aux[50];
    char wordT[50];
    bzero(wordT, 50);
    char holder, temp;

    int i=0, j=0, x=0, wordLen=0;
    while((holder = fgetc(playerState)) != EOF) {
        holder = toupper(holder);
        if (holder == ' ') break;
        wordT[i] = holder;
        i++;
        wordLen++;
    }
    fclose(playerState);

    wordLen--;


    playerState = fopen(path, "r");
    // verificar erros

    if (strcmp(word, wordT) != 0) {
        
        playerState = fopen(path, "r");
        int currentErrors = 0;
        // verificar erros
 
        while((holder = fgetc(playerState)) != EOF) {
            holder = toupper(holder);
            if (holder == 'G') {
                currentErrors++;
            }
        }

        int errors;
        wordLen++;
        if (wordLen <= 6) { errors = 7; }
        else if (wordLen > 6 && wordLen < 11) { errors = 8; }
        else if (wordLen >= 11) { errors = 9; }

        if (currentErrors >= errors) {
            udpSend(udpSocket, "RWG OVR\n", verbose);

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
        playerState = fopen(path, "a");
        fprintf(playerState, "%s",line);
        fclose(playerState);
        return true;
    }

    playerState = fopen(path, "a");
    char message[BUFF_SIZE];

    fprintf(playerState, "%s",line);
    fclose(playerState);

    if (strcmp(word, wordT) == 0) {
        sprintf(message, "RWG WIN %s", trial);
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
                udpSend(udpSocket, "RQT ERR\n", verbose);
                return true;    
            }
            ++plid;
        }
        plid = plid - 6;
    }
    else {
        udpSend(udpSocket, "RQT ERR\n", verbose);
        return true;
    }

    
    char path[30];
    sprintf(path, "Server/GAMES/GAME_%s.txt", plid);

    if (access(path, F_OK) != 0) {
        return true;
    }

    char newPath[50];
    char newFileName[50];
    sprintf(newPath, "Server/GAMES/%s", plid);
    if (access(newPath, F_OK) != 0) {
        mkdir(newPath, 0700);
    }

    mkdir(newPath, 0700);

    FILE* playerState = fopen(path, "r");
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    sprintf(newPath, "Server/GAMES/%s/%d%d%d_%d%d%d_Q.txt", plid, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    playerState = fopen(path, "r");

    rename(path, newPath);


    fclose(playerState);
    udpSend(udpSocket, "RQT OK\n", verbose);
    return true;
}