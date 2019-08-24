/*
 ============================================================================
 Name        : Simple Remote Execution System Win64 Server
 Author      : A
 Version     : 0.9v
 Copyright   : No copyright
 Description : 2803ICT assignment 1, Ansi-style, CLion + Ubuntu
 ============================================================================
 */

#include "server.h"

SOCKET connector(); // Initial socket connection
int listener(SOCKET sock); //  Handling the in coming clients
void sigChld(int signo); //Zombie removal
void closeConnection(SOCKET sock); // clean up socket

/* main entry */
int main(void) {
    setbuf(stdout, NULL);

    SOCKET sock = connector();
    listener(sock);

    return EXIT_SUCCESS;
}

/* Initial socket connection */
SOCKET connector(){
/* Initialize Winsock. */
#ifdef WIN64
    WSADATA wsaData;
	check(WSAStartup(MAKEWORD(2, 2), &wsaData) == NO_ERROR, "[ERROR] WSAStartup failed... \n");
#endif
    SOCKET listenFd;
    int sockOpt = 1;

    /* Initial socket address */
    SOCKADDR_IN addrServer;
    memset(&addrServer, 0, sizeof(addrServer));
    addrServer.sin_family = AF_INET;
    addrServer.sin_addr.s_addr = htonl(INADDR_ANY);
    addrServer.sin_port = htons(DEFAULTPORT);

    /* create socket */
    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    check(listenFd > 0, "[Error] Initialize socket failed... \n");
    printf("[INFO] Server socket created: %d... \n", listenFd);

    /* socket setting */
    setsockopt(listenFd, SOL_SOCKET, SO_KEEPALIVE, &sockOpt, sizeof(sockOpt));
    setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &sockOpt, sizeof(sockOpt));
    setsockopt(listenFd, IPPROTO_TCP, TCP_NODELAY, &sockOpt, sizeof(sockOpt));
    printf("[INFO] Socket setting ... done. \n");

    /* binding & listening */
    check(bind(listenFd, (SOCKADDR *) & addrServer, sizeof (addrServer)) == 0, "[Error] Binding socket failed... \n");
    printf("[INFO] Socket binding on port: %d... \n", DEFAULTPORT);
    // Listen for incoming connection requests on the created socket.
    check(listen(listenFd, SOMAXCONN) == 0, "[ERROR] Socket listening failed... \n");
    printf("[INFO] socket start listening on port %d... \n\n", DEFAULTPORT);

    return listenFd;

    error:
    closeConnection(listenFd);
    exit(EXIT_FAILURE);
}

/* Handling the in coming clients */
int listener(SOCKET server) {
    /* remote client address */
    SOCKADDR_IN addrClient;
    int len = sizeof(SOCKADDR);

#ifdef WIN64
    STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	memset(&pi, 0, sizeof(pi));
#else
    /* process id */
    pid_t pid;
    signal(SIGCHLD, sigChld);
#endif

    printf("[INFO] System is waiting for the client...\n");

    while (1) {
        /* accept clients */
        SOCKET client = accept(server, (SOCKADDR*)&addrClient, &len);
        check(client > 0, "[Error] accept function failed... \n");
        printf("[INFO] New client [ %d ] is connected... \n", client);
#ifdef WIN64
        /* create process (Win64) */
		check(!CreateProcess(NULL,   // No module name (use command line)
			NULL,   // Command line
			NULL,   // Process handle not inheritable
			NULL,   // Thread handle not inheritable
			FALSE,  // Set handle inheritance to FALSE
			0,      // No creation flags
			NULL,   // Use parent's environment block
			NULL,   // Use parent's starting directory
			&si,    // Pointer to STARTUPINFO structure
			&pi),   // Pointer to PROCESS_INFORMATION structure
			"[ERROR] CreateProcess failed. \n");

		// request handling
		clientHandle(clientSocket);

		// Wait until child process exits.
		WaitForSingleObject(pi.hProcess, INFINITE);

		// Close process and thread handles.
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
#else
        /* create process on Ubuntu */
        pid = fork();
        check(pid != -1, "[ERROR] Create fork failed...\n");

        if (pid == 0) { // child process
            closeConnection(server);
            recv_request(client); // request handling
            return 0;
        }
        else  //parents process
            closeConnection(client);//parents process

#endif
    }
    error:
    closeConnection(server);
    return -1;
}

/* Zombie removal*/
void sigChld(int signo){
#ifdef WIN64
    //do nothing
#else
    pid_t pid;
    int stat;
    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("[INFO] child %d of client terminated. \n", (int)pid);
    return;
#endif
}

/* clean up socket */
void closeConnection(SOCKET sock){
#ifdef WIN64
    closesocket(sock);
	WSACleanup(); //Clean up Winsock
#else
    close(sock);
#endif

}

/* receive message */
int recv_Msg(SOCKET sockFd, Buffer *buf){
    int nCount;
    char *sockData;
    sockData = (char *)malloc(sizeof(char) * STDBUF);
    memset(sockData, 0, sizeof(char) * STDBUF);

    while(1){
        nCount = recv(sockFd, sockData, STDBUF, 0);
        buffer_append(buf, sockData, strlen(sockData));
        memset(sockData, 0, sizeof(char) * STDBUF);

        if(nCount < STDBUF)
            break;
    }

    free(sockData);
    return 0;
}

/* send message */
int send_Msg(SOCKET sockFd, Buffer *buf){
    if(send(sockFd, buf->data, buf->size, 0) == -1){
        perror("[ERROR] send failed ");
        return -1;
    }
    return 0;
}