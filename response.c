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
int sendResponse(Response *resp, Buffer *buf);

int make_response(Request *req, SOCKET sock) {
    Response resp;
    Buffer buf;

    memset(&resp, 0, sizeof(Response));
    buffer_init(&buf);

    resp.timeStamp = req->timeStamp;
    resp.ptype = req->ptype;

    switch(req->ptype){
        case PUT : put(req, &resp); break;
        case GET : get(req, &resp); break;
        case RUN : run(req, &resp); break;
        case LIST : list(req, &resp); break;
        case SYS : sys(req, &resp); break;
        case QUIT : quit(req, &resp); break;
        default : break;
    }

    sendResponse(&resp, &buf);

    /* debug */
    printf("Server send: %s\n", buf.data);

    send_Msg(sock, &buf);
    buffer_free(&buf);

    return 1;
}

int put(Request *req, Response *resp){
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
    printf("[SERVER] Client socket %d closing...");
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

int sendResponse(Response *resp, Buffer *buf){
    buffer_append_timestamp(buf, resp->timeStamp);
    buffer_append_short(buf, resp->ptype);
    buffer_append_short(buf, resp->success);
    buffer_append(buf, resp->response, strlen(resp->response));
    return 0;
}