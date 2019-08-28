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

        /* 2, decomposition data */
        recvRequest(&req, &buf);

        /* 3, receive attachment if necessary */
        if(req.ptype == PUT)
            for(i = 0; i< req.files; i++){
                printf("[Server] Server receiving file: %s in dir: %s. \n", req.filev[i], req.dirname);
                buffer_init(&req.attachment[i]);
                recv_Msg(sockFd, &req.attachment[i]);
            }

        /* 4, response */
        if(make_response(&req, sockFd) == 0)
            loop = 0;

        /* free memory */
        buffer_free(&buf);
        requestFree(&req);
    }
#ifdef WIN32
    _close(sockFd);
#else
    close(sockFd);
#endif
    return 0;
}

void requestInit(Request *req){
    int i;
    memset(req, 0, sizeof(Request));
    req->dirname = (char *)malloc(sizeof(char) * 64);
    memset(req->dirname, 0, sizeof(char) * 64);

    for(i = 0; i < 10; i++){
        req->argv[i] = (char *)malloc(sizeof(char) * 64);
        memset(req->argv[i], 0, sizeof(char) * 64);
        req->filev[i] = (char *)malloc(sizeof(char) * 64);
        memset(req->filev[i], 0, sizeof(char) * 64);
        buffer_init(req->attachment);
    }
}

void requestFree(Request *req){
    int i;
    free(req->dirname);

    for(i = 0; i < 10; i++){
        free(req->argv[i]);
        free(req->filev[i]);
        buffer_free(&req->attachment[i]);
    }
}

/* 2, decomposition data */
int recvRequest(Request *req, Buffer *buf){
    int nCount = 0, i;
    char *tmp;
    tmp = (char *) malloc(sizeof(char) * 64);
    memset(tmp, 0 ,sizeof(char) * 64);

    /* read timestamp */
    if(sscanf(buf->data, "%13s", tmp) > 0){
        req->timeStamp = atoll(tmp);
        memset(tmp, 0 ,sizeof(char) * 64);
        nCount += 13;
    }

    /* read ptype */
    if(sscanf(buf->data + nCount++, "%1s", tmp) > 0){
        req->ptype = (short)atoi(tmp);
        memset(tmp, 0 ,sizeof(char) * 64);
    }

    /* read fmode */
    if(sscanf(buf->data + nCount++, "%1s", tmp) > 0){
        req->fmode = (short)atoi(tmp);
        memset(tmp, 0 ,sizeof(char) * 64);
    }

    /* read lmode */
    if(sscanf(buf->data + nCount++, "%1s", tmp) > 0){
        req->lmode = (short)atoi(tmp);
        memset(tmp, 0 ,sizeof(char) * 64);
    }

    /* read args */
    if(sscanf(buf->data + nCount++, "%1s", tmp) > 0){
        req->args = (short)atoi(tmp);
        memset(tmp, 0 ,sizeof(char) * 64);
    }

    /* read files */
    if(sscanf(buf->data + nCount++, "%1s", tmp) > 0){
        req->files = (short)atoi(tmp);
        memset(tmp, 0 ,sizeof(char) * 64);
    }

    /* read dirname */
    if(sscanf(buf->data + nCount, "%[^&]", tmp) > 0){
        snprintf(req->dirname, strlen(tmp) + 1, "%s", tmp);
        nCount += strlen(tmp) + 1;
        memset(tmp, 0 ,sizeof(char) * 64);
    }

    /* read argv if necessary */
    for(i = 0; i < req->args; i++){
        if(sscanf(buf->data + nCount, "%[^&]", tmp) > 0){
            snprintf(req->argv[i], strlen(tmp) + 1, "%s", tmp);
            nCount += strlen(tmp) + 1;
            memset(tmp, 0 ,sizeof(char) * 64);
        }
    }

    /* read files if necessary */
    for(i = 0; i < req->files; i++){
        if(sscanf(buf->data + nCount, "%[^&]", tmp) > 0){
            snprintf(req->filev[i], strlen(tmp) + 1, "%s", tmp);
            nCount += strlen(tmp) + 1;
            memset(tmp, 0 ,sizeof(char) * 64);
        }
    }

    free(tmp);
    return 0;
}