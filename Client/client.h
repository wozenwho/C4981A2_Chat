#pragma once

#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define BUFLEN 80

typedef struct
{
    int sd;
    FILE * fp;
} SaveArg;

int CreateSock();
int ConnectToServer(int sd, char* host, int port);
void* RecvThreadFunc(void* ptr);
void* SendThreadFunc(void* ptr);
void* RecvThreadFuncSave(void* ptr);
void* SendThreadFuncSave(void* ptr);