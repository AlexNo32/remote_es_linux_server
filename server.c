//
// Created by charname on 7/9/19.
//

#include "server.h"
#include "func.h"

int  sockCreate();
void sockOpt(SOCKET listenFd);
int  sockListening(SOCKET listenFd);
int  clientHandler(SOCKET listenFd);
void sigChld(int signo);

int initSock(){
    SOCKET listenFd;
    listenFd = sockCreate();
    if(listenFd == -1)
        return -1;

    printf("[INFO] server socket created: %d. \n", listenFd);

    sockOpt(listenFd);
    printf("[INFO] socket setting ... done. \n");

    if(sockListening(listenFd) == -1)
        return -1;

    printf("[INFO] socket start listening on port %d. \n\n", DEFAULTPORT);

    clientHandler(listenFd);
    return 0;
}

/* create socket */
int sockCreate(){
    SOCKET listenFd;

    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenFd < 0){
        printf("[ERROR] initialize socket failed. \n");
        return -1;
    }
    return listenFd;
}

/* socket setting */
void sockOpt(SOCKET listenFd){
    int sockOpt = 1;

    if(setsockopt(listenFd, SOL_SOCKET, SO_KEEPALIVE, &sockOpt, sizeof(sockOpt)) == -1)
        printf("[ERROR] socket set KEEPLIVE failed. \n");

    if(setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &sockOpt, sizeof(sockOpt)) == -1)
        printf("[ERROR] socket set ADDR_REUSE failed. \n");

    if(setsockopt(listenFd, IPPROTO_TCP, TCP_NODELAY, &sockOpt, sizeof(sockOpt)) == -1)
        printf("[ERROR] socket set NODELAY failed. \n");
}

/* binding & listening */
int sockListening(SOCKET listenFd){
    SOCKADDR_IN addrServer;
    memset(&addrServer, 0, sizeof(addrServer));
    addrServer.sin_family = AF_INET;
    addrServer.sin_addr.s_addr = htonl(INADDR_ANY);
    addrServer.sin_port = htons(DEFAULTPORT);

    if(bind(listenFd, (SOCKADDR *) & addrServer, sizeof (addrServer)) == -1){
        printf("[ERROR] socket binding failed. \n");
        close(listenFd);
        return -1;
    }

    if(listen(listenFd, SOMAXCONN) == -1){
        printf("ERROR] socket listening failed. \n");
        close(listenFd);
        return -1;
    }
}

/* Handling coming clients */
int clientHandler(SOCKET sock) {
    SOCKADDR_IN addrClient;
    int len = sizeof(SOCKADDR);

    pid_t pid;
    signal(SIGCHLD, sigChld);

    printf("[INFO] system is waitting for the client...\n");

    while (1) {
        SOCKET clientFd = accept(sock, (SOCKADDR*)&addrClient, &len);
        if(clientFd == -1){
            printf("[Error] client accept failed.\n");
            continue;
        }
        printf("[INFO] New client [ %d ] is connected. \n", clientFd);

        pid = fork();
        if(pid == -1){
            printf("[ERROR] fork create failed.\n");
            continue;
        }

        // child process
        if (pid == 0) {
            // request handling
            //clientHandle(clientFd);

            close(clientFd);
            return 0;
        }
            //parents process
        else
            close(clientFd);
    }
    return 0;
}

/* Zombie removal*/
void sigChld(int signo){
    pid_t pid;
    int stat;
    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("[INFO] child %d of client is terminated. \n", (int)pid);
    return;
}