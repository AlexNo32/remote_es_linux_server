/*
 ============================================================================
 Name        : Simple Remote Execution System Linux server
 Author      : Alex
 Version     : 0.9v
 Copyright   : No copyright
 Description : 2803ICT assignment 1, Ansi-style, CLion + Ubuntu
 ============================================================================
 */

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <signal.h>
#include <sys/wait.h>

#include "stringbuffer.h"

#define DEFAULTPORT  1024
#define STDBUF 1024

typedef unsigned int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

int initSock();
int clientHandle(SOCKET sock);
int recvMsg(SOCKET sockFd, Vector *vector);
int sendMsg(SOCKET sockFd, Vector *vector);

#endif //SERVER_SERVER_H
