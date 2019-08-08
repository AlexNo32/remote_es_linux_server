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

int main(void){
    setbuf(stdout, NULL);
    initSock();
    return EXIT_SUCCESS;
}

