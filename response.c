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

int put(Request *req, Response *resp);
int get(Request *req, Response *resp);
int run(Request *req, Response *resp);
int list(Request *req, Response *resp);
int sys(Request *req, Response *resp);
int quit(Request *req, Response *resp);

int execution(char* cmd, Buffer* buf);
int fillResponse(Response *resp, Buffer *buf);
int removedir(char *dirname);
int storeFile(char *filename, char *content);

void responseInit(Response *resp);
void responseFree(Response *resp);

/* make a response (server replay to client) */
int make_response(Request *req, SOCKET sock) {
    /* 1, init */
    int q = 1;
    Response resp;
    Buffer buf;
    responseInit(&resp);
    buffer_init(&buf);

    resp.timeStamp = req->timeStamp;
    resp.ptype = req->ptype;

    /* 2, response by methods */
    switch(req->ptype){
        case PUT :
            put(req, &resp);
            break;
        case GET :
            get(req, &resp);
            break;
        case RUN :
            run(req, &resp);
            break;
        case LIST :
            list(req, &resp);
            break;
        case SYS :
            sys(req, &resp);
            break;
        case QUIT :
            quit(req, &resp);
            printf("[Server] Client socket %d closing...\n");
            q = 0;
            break;
        default :
            break;
    }

    /* ready to send response */
    fillResponse(&resp, &buf);
    send_Msg(sock, &buf);

    /* debug */
    printf("Server send: %s\n", buf.data);

    /* clean memory */
    buffer_free(&buf);
    responseFree(&resp);
    return q;
}

void responseInit(Response *resp){
    memset(resp, 0, sizeof(Response));
    resp->response = malloc(5120);
}

void responseFree(Response *resp){
    free(resp->response);
}

int put(Request *req, Response *resp){
    int i;
    char *fPath;
    fPath = (char *) malloc(sizeof(char) * 128);
    memset(fPath, 0, sizeof(char) * 128);
    /* 1, make a project folder if not exist */
    if (access(req->dirname, 0) == -1)
        mkdir(req->dirname, 0777);

    else if(req->fmode){
        removedir(req->dirname);
        mkdir(req->dirname, 0777);
    }
    /* 2, save files */
    for(i = 0; i < req->files; i++){
        snprintf(fPath, 128, "%s/%s", req->dirname, req->filev[i]);
        storeFile(fPath, req->attachment[i].data);
        memset(fPath, 0, sizeof(char) * 128);
    }

    free(fPath);

    /* 3, fill response */
    resp->success = 1;
    resp->response = "[Server] Upload success.\n";

    return 0;
}

int get(Request *req, Response *resp){
    return 0;
}

int run(Request *req, Response *resp){
    return 0;
}

int list(Request *req, Response *resp){
    return 0;
}

int sys(Request *req, Response *resp){
#ifdef WIN64
    char *commSet[3] = { "hostname", "ver", "wmic cpu get name | find /v \"Name\"" };
#else
    char *commSet[3] = { "hostname", "cat /proc/version", "cat /proc/cpuinfo | grep 'model name' | cut -d ':' -f 2 | uniq" };
#endif
    Buffer buf;
    buffer_init(&buf);

    int i;
    for(i = 0; i < 3; i++){
        execution(commSet[i], &buf);
        buffer_append(&buf, "&", 1);
    }

    memcpy(resp->response, buf.data, buf.size);
    buffer_free(&buf);
    return 0;
}

int quit(Request *req, Response *resp){
    resp->success = 1;
    return 0;
}

/* exec command line */
int execution(char* cmd, Buffer* buf) {
    char *temp;
    temp = (char*) malloc (sizeof(char) * 125);

#ifdef WIN64
    FILE* pipe = _popen(cmd, "r");
#else
    FILE *pipe = popen(cmd, "r");
#endif

    if (!pipe)
        return 0;

    while (!feof(pipe)) {
        if (fgets(temp, 125, pipe)) {
            buffer_append(buf, temp, strlen(temp));
        }
    }
#ifdef WIN64
    _pclose(pipe);
#else
    pclose(pipe);
#endif
    free(temp);
    return 0;
}

/* rmdir */
int removedir(char *dirname) {
    char *del;
    del = malloc(sizeof(char) * 128);
    memset(del, 0, sizeof(char) * 128);
    snprintf(del, 128, "rm -r -f %s", dirname);

    execution(del, NULL);

    free(del);
    return 0;
}

/* store files */
int storeFile(char *filename, char *content){
    FILE *fptr;

    fptr = fopen(filename, "rw");
    fprintf(fptr,"%s", content);

    fclose(fptr);
    return 0;
}

int fillResponse(Response *resp, Buffer *buf){
    buffer_append_timestamp(buf, resp->timeStamp);
    buffer_append_short(buf, resp->ptype);
    buffer_append_short(buf, resp->success);
    buffer_append(buf, resp->response, strlen(resp->response));
    buffer_append(buf, "&", 1);
    return 0;
}
