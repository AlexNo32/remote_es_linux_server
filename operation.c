/*
 ============================================================================
 Name        : Simple Remote Execution System Linux server
 Author      : Alex
 Version     : 0.9v
 Copyright   : No copyright
 Description : 2803ICT assignment 1, Ansi-style, CLion + Ubuntu
 ============================================================================
 */
#include "server.h"

/* Request handling */
int clientHandle(SOCKET sockFd) {
    int loop = 1;

    // keep recving message form one client until receive 'quit'
    while (loop) {
        Vector vector;
        vector_init(&vector);

        recvMsg(sockFd, &vector);

        printf("Receive from client: %s\n", vector.data);
//        char buffer[PACKAGESIZE] = { 0 };
//        int offset = 0, rCode = 0;
//        unsigned long timeStamp = 0;
//        int i;

//        // recv data
//        if (recv(sock, buffer, PACKAGESIZE, 0) < 0) {
//            printf("[Error] Lost connection. \n");
//            loop = 0;
//        }


    }
    close(sockFd);
    return 0;
}

int recvRequest(){
    return 0;
}

int sendResponse(){
    return 0;
}
