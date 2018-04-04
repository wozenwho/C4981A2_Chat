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
    char* host = "127.0.0.1";
    int port = 7000;
    while((opt = getopt(argc, argv, OPTIONS)) != -1)
    {
        switch (opt)
        {
            case 'h':
            host = optarg;
            break;

            case 'p':
            port = atoi(optarg);
            break;

            case 'f':
            saveFlag = 1;
            printf("save flag is set\n");
            break;

        }
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