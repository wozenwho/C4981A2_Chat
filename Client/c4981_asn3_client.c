/*------------------------------------------------------------------------------
--	Source File:	c4981_asn3_client.c
--
--	Program:		Chat Client
--
--  Functions       
--
--	Date:			Apr 4 2018
--
--	Designer:		Jeffrey Chou
--
--	Programmer:		Jeffrey Chou
--
--	Notes:
--	Chat client that connects to a server passed in via command line arguments.
--  Creates a thread for receiving from the server and for getting input from the
--  user. 
--
--  Contains the main driver of the client program
--
-------------------------------------------------------------------------------*/

#include "client.h"

#define OPTIONS "?h:p:f"

int main(int argc, char* argv[])
{
    char filename[32];
    int opt, saveFlag = 0;
    FILE* fp = 0;
    if(argc <= 3)
    {
        fprintf(stderr, "usage: %s [-h ip addr] [-p port number] <-f>\n", argv[0]);
        return 1;
    }
    char* host = 0;
    int port = -1;
    while((opt = getopt(argc, argv, OPTIONS)) != -1)
    {
        switch (opt)
        {
            case 'h':
            host = optarg;
            break;

            case 'p':
            port = atoi(optarg);
            if(port < 0)
            {
                fprintf(stderr, "Invalid port number\n");
                return 1;
            }
            break;

            case 'f':
            saveFlag = 1;
            printf("save flag is set\n");
            break;

        }
    }

    if(host == 0)
    {
        fprintf(stderr, "host ip address is mandatory\n");
        return 1;
    }

    if(port == -1)
    {
        fprintf(stderr, "port number is mandatory\n");
        return 1;
    }
        
    pthread_t recvThread, sendThread;
    int sd = 0;
    sd = CreateSock();
    if(sd < 0)
    {
        fprintf(stderr, "Could not create socket\n");
        return -1;
    }
    int ret = ConnectToServer(sd, host, port);
    if(ret < 0)
    {
        fprintf(stderr, "Connection to Server Failed\n");
        close(sd);
        return -1;
    }
    if(saveFlag)
    {
        sprintf(filename, "%s_room_history.txt", host);
        fp = fopen(filename, "a");
    }
    fprintf(stdout, "Connected to %s\n", host);
    system("stty -echo");
    if(fp)
    {
        SaveArg arg;
        arg.sd = sd;
        arg.fp = fp;
        pthread_create(&recvThread, NULL, RecvThreadFuncSave, &arg);
        pthread_create(&sendThread, NULL, SendThreadFuncSave, &arg);
    }
    else
    {
        pthread_create(&recvThread, NULL, RecvThreadFunc, &sd);
        pthread_create(&sendThread, NULL, SendThreadFunc, &sd);
    }
    pthread_join(sendThread,NULL);
    close(sd);
    if(fp)
    {
        fclose(fp);
    }
    system("stty echo");
    return 0;
}