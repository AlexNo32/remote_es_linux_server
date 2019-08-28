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
int getFileSize(FILE * fp);
int loadFile(char *fPath, Buffer *buf);

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
            printf("[Server] Client socket %d closing...\n", sock);
            q = 0;
            break;
        default :
            break;
    }

    /* ready to send response */
    fillResponse(&resp, &buf);
    send_Msg(sock, &buf);

    /* clean memory */
    buffer_free(&buf);
    responseFree(&resp);
    return q;
}

void responseInit(Response *resp){
    memset(resp, 0, sizeof(Response));
    resp->response = (char *)malloc(sizeof(char) * 1024 * 10);
    memset(resp->response, 0, sizeof(char) * 1024 * 10);
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
#ifdef WIN32
    if (_access(req->dirname, 0) == -1)
		_mkdir(req->dirname, 0777);
	else if (req->fmode) {
		removedir(req->dirname);
		_mkdir(req->dirname, 0777);
}
#else
    if (access(req->dirname, 0) == -1)
        mkdir(req->dirname, 0777);
    else if (req->fmode) {
        removedir(req->dirname);
        mkdir(req->dirname, 0777);
    }
#endif // WIN32

    /* 2, save files */
    for(i = 0; i < req->files; i++){
        snprintf(fPath, 128, "./%s/%s", req->dirname, req->filev[i]);
        storeFile(fPath, req->attachment[i].data);
        memset(fPath, 0, sizeof(char) * 128);
    }

    free(fPath);
    /* 3, fill response */
    resp->success = 1;
    char *reply = "[INFO] Upload success...\n";
    snprintf(resp->response, strlen(reply), "%s", reply);

    printf("[Server] Request 'put' response %ld\n", strlen(resp->response));

    return 0;
}

int get(Request *req, Response *resp){
    Buffer buf;
    char * fPath;
    buffer_init(&buf);

    /* 1, fetch file */
    fPath = (char *)malloc(sizeof(char) * 128);
    memset(fPath, 0 ,sizeof(char) * 128);
    snprintf(fPath, 128, "./%s/%s", req->dirname, req->filev[0]);

    /* 2, if file exist, load file */
    if(loadFile(fPath, &buf) == 0)
        snprintf(resp->response, buf.size, "%s", buf.data);
    else{
        /* 3, if not exist, return msg */
        char *reply = "[INFO] Load file exception...\n";
        snprintf(resp->response, strlen(reply), "%s", reply);
    }

    resp->success = 1;
    free(fPath);
    buffer_free(&buf);

    printf("[Server] Request 'get' response %ld\n", strlen(resp->response));

    return 0;
}

int run(Request *req, Response *resp){

    printf("[Server] Request 'run' response %ld\n", strlen(resp->response));
    return 0;
}

int list(Request *req, Response *resp){
#ifdef WIN32
    char* cmdSet[2] = { "dir ", "icacls"};
#else
    char* cmdSet[2] = { "ls ", "ls -l " };
#endif // WIN32


    char *cmd;
    int n = 0;
    Buffer buf;
    buffer_init(&buf);

    cmd = (char *) malloc(sizeof(char) * 64);
    memset(cmd, 0, sizeof(char) * 64);

#ifdef WIN32

    if (req->lmode) {// icalcs
		if (req->dirname == "")
			snprintf(cmd, strlen(cmdSet[1]) + 1, "%s *", cmdSet[1]);
		else
			snprintf(cmd, strlen(cmdSet[1]) + strlen(req->dirname) + 1, "%s %s\*", cmdSet[1], req->dirname);

	}else {// dir
		if (req->dirname == "") {
			snprintf(cmd, strlen(cmdSet[0]) + 1,"%s", cmdSet[0]);
		}else {
			snprintf(cmd, strlen(cmdSet[1]) + strlen(req->dirname) + 1, "%s %s", cmdSet[0], req->dirname);
		}
	}

#else
    strcat(cmd, cmdSet[req->lmode]);
    if (req->dirname != "")
        strcat(cmd, req->dirname);
#endif // WIN32
    execution(cmd, &buf);

    snprintf(resp->response, buf.size, "%s", buf.data);
    resp->success = 1;

    buffer_free(&buf);
    free(cmd);
    printf("[Server] Request 'list' response %ld\n", strlen(resp->response));
    return 0;
}

int sys(Request *req, Response *resp){
    char *exp[3] = {"Computer name: ", "\nOperation System: ", "\nCPU type: "};

#ifdef WIN32
    char *cmdSet[3] = { "hostname", "ver", "wmic cpu get name | find /v \"Name\"" };
#else
    char *cmdSet[3] = { "hostname", "cat /proc/version", "cat /proc/cpuinfo | grep 'model name' | cut -d ':' -f 2 | uniq" };
#endif
    Buffer buf;
    buffer_init(&buf);

    int i;
    for(i = 0; i < 3; i++){
        buffer_append(&buf, exp[i], strlen(exp[i]));
        execution(cmdSet[i], &buf);
    }

    snprintf(resp->response, buf.size + 1, "%s", buf.data);
    resp->success = 1;

    buffer_free(&buf);
    printf("[Server] Request 'sys' response %ld\n", strlen(resp->response));
    return 0;
}

int quit(Request *req, Response *resp){
    resp->success = 1;
    return 0;
}

/* exec command line */
int execution(char* cmd, Buffer* buf) {
    char *temp;
    temp = (char*) malloc (sizeof(char) * 512);
    memset(temp, 0, sizeof(char) * 512);
#ifdef WIN32
    FILE* pipe = _popen(cmd, "r");
#else
    FILE *pipe = popen(cmd, "r");
#endif

    if (!pipe)
        return 0;

    while (!feof(pipe)) {
        if (fgets(temp, 512, pipe)) {
            buffer_append(buf, temp, strlen(temp));
        }
    }
#ifdef WIN32
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

    fptr = fopen(filename,"wb");
    fwrite(content, 1, strlen(content), fptr);

    fclose(fptr);
    return 0;
}

/* fetch file */
int loadFile(char *fPath, Buffer *buf){
    FILE *fptr;
    int fileLen, nCount;
    char *fTemp;

    fptr = fopen(fPath, "rb");
    if (fptr == NULL)
        return -1;

    fileLen = getFileSize(fptr);
    fTemp = (char*) malloc (sizeof(char) * fileLen);
    memset(fTemp, 0, sizeof(char) * fileLen);

    if (fTemp == NULL)
        return -1;

    nCount = fread (fTemp, 1, fileLen, fptr);
    if (nCount != fileLen)
        return -1;
    buffer_append(buf, fTemp, fileLen);

    fclose(fptr);
    free(fTemp);

    return 0;
}

int fillResponse(Response *resp, Buffer *buf){
    //char itc[10];
    buffer_append_timestamp(buf, resp->timeStamp);
    buffer_append_short(buf, resp->ptype);
    buffer_append_short(buf, resp->success);
    buffer_append_int(buf, strlen(resp->response));
    buffer_append(buf, "&", 1);
    buffer_append(buf, resp->response, strlen(resp->response));
    return 0;
}

/* return file length */
int getFileSize(FILE * fp) {
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    return size;
}