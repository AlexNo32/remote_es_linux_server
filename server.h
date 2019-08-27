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
#include <errno.h>
#include "stringbuffer.h"

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#include <io.h>
#pragma comment(lib,"Ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/stat.h>

typedef unsigned int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
#endif

#define DEFAULTPORT  1024
#define STDBUF 1024

#define check(A, M, ...) if(!(A)) { perror(M); errno = 0; goto error;}

/******************** struct *********************/
enum PTYPE{
    PUT = 4, GET, RUN, LIST, SYS, QUIT
};

typedef struct {
    unsigned long timeStamp;// length: 13
    unsigned short ptype; // [4 - 9]
    unsigned short fmode; // 1: On, 0: Off
    unsigned short lmode; // 1: On, 0: Off
    char * dirname; // folder name
    unsigned short args;
    unsigned short files;
    char *argv[10];
    char *filev[10];
    Buffer attachment[10];
} Request;

typedef struct {
    unsigned long timeStamp; // length :13
    unsigned short ptype; // [4 - 9]
    unsigned short success; // 1/0 cmd execute success or not
    char *response; // response message
} Response;
/******************** struct end  ****************/

/***********  function *****************/
int recv_request(SOCKET sock);
int make_response(Request *req, SOCKET sock);

int recv_Msg(SOCKET sockFd, Buffer *buf);
int send_Msg(SOCKET sockFd, Buffer *buf);
/*********** function end **************/

#endif //SERVER_SERVER_H
