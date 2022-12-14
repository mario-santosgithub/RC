#include "server.h"
#include "../common.h"



bool start(int udpSocket, char* plid, bool verbose){
    if (verbose)
        printf("PLID: %s\n", plid);
    printf("p: %s\n", plid);
    // Check if Plid only has digits
    if (strlen(plid) == 6) {
        while (*plid) {
            if (*plid < '0' || *plid > '9') { 
                udpSend(udpSocket, "RSG NOK\n", verbose);
                return true;    
            }
            ++plid;
        }
        plid = plid - 6;
    }
    else {
        udpSend(udpSocket, "RSG NOK\n", verbose);
        return true;
    }
    
    udpSend(udpSocket, "RSG OK 10 8\n", verbose);
    return true;
}