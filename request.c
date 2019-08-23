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

int recvRequest(Request *req, Buffer *buf);

void requestInit(Request *req);
void requestFree(Request *req);

/* Request handling */
int recv_request(SOCKET sockFd) {
    int loop = 1;

    // keep recving message form one client until receive 'quit'
    while (loop) {
        int i;
        Request req;
        Buffer buf;
        requestInit(&req);
        buffer_init(&buf);

        /* 1, receive data */
        recv_Msg(sockFd, &buf);

        /* debug */
        printf("server receive %s\n", buf.data);

        /* 2, decomposition data */
        recvRequest(&req, &buf);
        buffer_free(&buf);

        /* 3, receive attachment if necessary */
        for(i = 0; i< req.files; i++){
            buffer_init(&buf);
            recv_Msg(sockFd, &buf);
            req.attachment[i] = buf;
//            buffer_free(&buf);
        }

        /* 4, response */
        if(make_response(&req, sockFd) == 0)
            loop = 0;

        /* free memory */
        requestFree(&req);
    }

    close(sockFd);
    return 0;
}

void requestInit(Request *req){
    int i;
    memset(req, 0, sizeof(Request));
    req->dirname = malloc(50);

    for(i = 0; i < 10; i++){
        req->argv[i] = malloc(50);
        req->filev[i] = malloc(50);
        buffer_init(req->attachment);
    }
}

void requestFree(Request *req){
    int i;
    free(req->dirname);

    for(i = 0; i < 10; i++){
        free(req->argv[i]);
        free(req->filev[i]);
        buffer_free(req->attachment);
    }
}

/* 2, decomposition data */
int recvRequest(Request *req, Buffer *buf){
    int nCount = 0, i;
    char *tmp;
    tmp = malloc(25);
    memset(tmp, 0 ,25);

    /* read timestamp */
    sscanf(buf->data, "%13s", tmp);
    req->timeStamp = atoll(tmp);
    memset(tmp, 0 ,13);
    nCount += 13;

    /* read ptype */
    sscanf(buf->data + nCount++, "%1s", tmp);
    req->ptype = (short)atoi(tmp);
    memset(tmp, 0 ,1);

    /* read fmode */
    sscanf(buf->data + nCount++, "%1s", tmp);
    req->fmode = (short)atoi(tmp);
    memset(tmp, 0 ,1);

    /* read lmode */
    sscanf(buf->data + nCount++, "%1s", tmp);
    req->lmode = (short)atoi(tmp);
    memset(tmp, 0 ,1);

    /* read args */
    sscanf(buf->data + nCount++, "%1s", tmp);
    req->args = (short)atoi(tmp);
    memset(tmp, 0 ,1);

    /* read files */
    sscanf(buf->data + nCount++, "%1s", tmp);
    req->files = (short)atoi(tmp);
    memset(tmp, 0 ,1);

    /* read dirname */
    sscanf(buf->data + nCount, "%[^&]", tmp);
    nCount += strlen(tmp);
    req->dirname = tmp;
    memset(tmp, 0 ,25);

    /* read argv if necessary */
    for(i = 0; i < req->args; i++){
        sscanf(buf->data + nCount, "%[^&]", tmp);
        nCount += strlen(tmp);
        req->argv[i] = tmp;
        memset(tmp, 0, 50);
    }

    /* read files if necessary */
    for(i = 0; i < req->files; i++){
        sscanf(buf->data + nCount, "%[^&]", tmp);
        nCount += strlen(tmp);
        req->filev[i] = tmp;
        memset(tmp, 0, 20);
    }

    free(tmp);
    return 0;
}



/* verify file 1. c or cpp source. 2 file exist. */
int fileExist(char * filePath) {

    if (access(filePath, 0) == -1) {
        printf("[ERROR] The program %s already exist.\n", filePath);
        return -1;
    }

    return 1;
}