//
// Created by charname on 7/9/19.
//

#ifndef SERVER_HEADER_H
#define SERVER_HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include <netinet/in.h>
#include <netinet/tcp.h>

#define check(A, M, ...) if(!(A)) { perror(M); errno = 0; goto error;}

typedef unsigned int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

#define DEFAULTPORT  1024

#define SUCCESS 1
#define ERROR -1
#define FATAL -2

/* System function */
SOCKET connector();
int listener(SOCKET sock);
void closeConnection(SOCKET socket);
void sigChld(int signo);

#endif //SERVER_HEADER_H
