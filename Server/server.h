#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <memory.h>

#include <string>
#include <iostream>
#include <cerrno>


#define PORT_NO         9999
#define BUFLEN          256
#define MAX_NUM_CLIENTS 16

void transmitMessage(int sockfd, size_t index, char* buffer);