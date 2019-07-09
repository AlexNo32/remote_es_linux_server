//
// Created by charname on 7/9/19.
//
#include "server.h"

/* Request handling */
int clientHandle(SOCKET sock) {
    int loop = 1;
/**
    // keep recving message form one client until receive 'quit'
    while (loop) {
        char buffer[PACKAGESIZE] = { 0 };
        int offset = 0, rCode = 0;
        unsigned long timeStamp = 0;
        int i;

        // recv data
        if (recv(sock, buffer, PACKAGESIZE, 0) < 0) {
            printf("[Error] Lost connection. \n");
            loop = 0;
            //return ERROR;
        }
        // get command code
        offset += USINT - 1;

        for (i = 0; i < offset; i++)
            rCode = rCode * 10 + (buffer[i] - '0');


        offset += TIMESTAMP;
        for (i = 2; i < offset; i++)
            timeStamp = timeStamp * 10 + (buffer[i] - '0');

        switch (rCode) {
            case N_PUT:
                sPut(sock, timeStamp, buffer + offset); break;

            case N_GET:
                sGet(sock, timeStamp, buffer + offset); break;

            case N_RUN:
                sRun(sock, timeStamp, buffer + offset); break;

            case N_LIST:
                sList(sock, timeStamp, buffer + offset); break;

            case N_SYS:
                sSys(sock, timeStamp); break;

            case N_QUIT:
                sQuit(sock);
                loop = 0;
                break;

            default:
                break;
        }

    }
*/
    return 0;
}
