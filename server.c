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

int  sockCreate();
void sockOpt(SOCKET listenFd);
void sockListening(SOCKET listenFd);
void clientHandler(SOCKET listenFd);
void sigChld(int signo);

int initSock(){
    SOCKET listenFd;

    listenFd = sockCreate();
    printf("[INFO] server socket created: %d. \n", listenFd);

    sockOpt(listenFd);
    printf("[INFO] socket setting ... done. \n");

    sockListening(listenFd);
    printf("[INFO] socket start listening on port %d. \n\n", DEFAULTPORT);

    clientHandler(listenFd);
    return 0;
}

/* create socket */
int sockCreate(){
    SOCKET listenFd;

    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenFd < 0){
        perror("[ERROR] initialize socket failed:");
        exit(EXIT_FAILURE);
    }
    return listenFd;
}

/* socket setting */
void sockOpt(SOCKET listenFd){
    int sockOpt = 1;

    if(setsockopt(listenFd, SOL_SOCKET, SO_KEEPALIVE, &sockOpt, sizeof(sockOpt)) == -1)
        perror("[ERROR] socket set KEEPLIVE failed:");

    if(setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &sockOpt, sizeof(sockOpt)) == -1)
        perror("[ERROR] socket set ADDR_REUSE failed:");

    if(setsockopt(listenFd, IPPROTO_TCP, TCP_NODELAY, &sockOpt, sizeof(sockOpt)) == -1)
        perror("[ERROR] socket set NODELAY failed:");
}

/* binding & listening */
void sockListening(SOCKET listenFd){
    SOCKADDR_IN addrServer;
    memset(&addrServer, 0, sizeof(addrServer));
    addrServer.sin_family = AF_INET;
    addrServer.sin_addr.s_addr = htonl(INADDR_ANY);
    addrServer.sin_port = htons(DEFAULTPORT);

    if(bind(listenFd, (SOCKADDR *) & addrServer, sizeof (addrServer)) == -1){
        perror("[ERROR] socket binding failed:");
        close(listenFd);
        exit(EXIT_FAILURE);
    }

    if(listen(listenFd, SOMAXCONN) == -1){
        perror("[ERROR] socket listening failed:");
        close(listenFd);
        exit(EXIT_FAILURE);
    }
}

/* Handling coming clients */
void clientHandler(SOCKET sock) {
    SOCKADDR_IN addrClient;
    int len = sizeof(SOCKADDR);

    pid_t pid;
    signal(SIGCHLD, sigChld);

    printf("[INFO] system waiting for the client...\n");

    while (1) {
        SOCKET clientFd = accept(sock, (SOCKADDR*)&addrClient, &len);
        if(clientFd == -1){
            perror("[Error] client accept failed:");
            continue;
        }
        printf("[INFO] New client [ %d ] is connected. \n", clientFd);

        pid = fork();
        if(pid == -1){
            perror("[ERROR] fork create failed:");
            continue;
        }

        // child process
        if (pid == 0) {
            // request handling
            //clientHandle(clientFd);
            // DEBUG
            //while(1){
                // 1. receive msg
                // 2. return msg
           // }


            close(clientFd);
            return;
        }
            //parents process
        else
            close(clientFd);
    }
}

/* Zombie removal*/
void sigChld(int signo){
    pid_t pid;
    int stat;
    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("[INFO] child %d of client is terminated. \n", (int)pid);
    return;
}


int recvMsg(SOCKET sockFd, char *msg){
    int nCount;
    char buff[STDBUF];

    while( (nCount = recv(sockFd, buff, STDBUF, MSG_WAITALL)) > 0 ){
        //fwrite(buffer, nCount, 1, fp);
    }
    // nbytes = recv(sockFd, buff, buff_size,MSG_WAITALL);
    return 0;
}

int sendMsg(SOCKET sockFd, char* msg){
    if(send(sockFd, msg, strlen(msg), MSG_WAITALL) == -1){
        perror("[ERROR] send failed ");
        return -1;
    }
    return 0;
}