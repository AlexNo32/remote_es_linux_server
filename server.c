//
// Created by charname on 7/7/19.
//
#include "header.h"

int main(void){
    setbuf(stdout, NULL);

    SOCKET sock = connector();
    listener(sock);

    return EXIT_SUCCESS;
}

/* get socket connection */
SOCKET connector(){
    SOCKET listenFd;
    int sockOpt = 1;

    SOCKADDR_IN addrServer;
    memset(&addrServer, 0, sizeof(addrServer));
    addrServer.sin_family = AF_INET;
    addrServer.sin_addr.s_addr = htonl(INADDR_ANY);
    addrServer.sin_port = htons(DEFAULTPORT);

    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    check(listenFd > 0, "[Error] Initialize socket failed. \n");
    printf("[INFO] Server socket created: %d. \n", listenFd);

    setsockopt(listenFd, SOL_SOCKET, SO_KEEPALIVE, &sockOpt, sizeof(sockOpt));
    setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &sockOpt, sizeof(sockOpt));
    setsockopt(listenFd, IPPROTO_TCP, TCP_NODELAY, &sockOpt, sizeof(sockOpt));

    check(bind(listenFd, (SOCKADDR *) & addrServer, sizeof (addrServer)) == 0, "[Error] Binding failed. \n");
    printf("[INFO] Socket bind on port: %d. \n", DEFAULTPORT);

    check(listen(listenFd, SOMAXCONN) == 0, "[ERROR] Listening failed. \n");
    printf("[INFO] Socket start listening... \n\n");

    return listenFd;

    error:
    closeConnection(listenFd);
    return ERROR;
}

/* Handling coming clients */
int listener(SOCKET sock) {
    SOCKADDR_IN addrClient;
    int len = sizeof(SOCKADDR);

    pid_t pid;
    signal(SIGCHLD, sigChld);

    while (1) {
        SOCKET clientSocket = accept(sock, (SOCKADDR*)&addrClient, &len);
        check(clientSocket > 0, "[Error] accept failed. \n");
        printf("[INFO] New client [ %d ] is connected. \n", clientSocket);

        // create process (linux)
        pid = fork();
        check(pid != -1, "[ERROR] Create fork failed.\n");

        // child process
        if (pid == 0) {
            // request handling
            clientHandle(clientSocket);

            closeConnection(clientSocket);
            return SUCCESS;
        }
        //parents process
        else
            closeConnection(clientSocket);
    }
    return SUCCESS;

    error:
    closeConnection(sock);
    return ERROR;
}

/* Zombie removal*/
void sigChld(int signo){
    pid_t pid;
    int stat;
    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("[INFO] child %d of client is terminated. \n", (int)pid);
    return;
}

/* clean up socket */
void closeConnection(SOCKET sock){
    close(sock);
}
