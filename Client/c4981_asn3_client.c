#include "client.h"

#define OPTIONS "?h:p:f"

int main(int argc, char* argv[])
{
    char filename[32];
    system("stty -echo");
    int opt, saveFlag = 0;
    FILE* fp = 0;
    if(argc < 3)
    {
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
            printf("flag is set\n");
            break;

        }
    }
    if(saveFlag)
    {
        sprintf(filename, "%s.txt", host);
        fp = fopen(filename, "a");
    }
        

    pthread_t recvThread, sendThread;
    int sd = 0;
    char sBuf[BUFLEN];
    sd = CreateSock();
    if(sd < 0)
        return -1;

    int ret = ConnectToServer(sd, host, port);
    if(ret < 0)
    {
        close(sd);
        return -1;
    }
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
    printf("exited from program\n");
    close(sd);
    fclose(fp);
    system("stty echo");
    return 0;
}