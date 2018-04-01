#include "client.h"

int CreateSock()
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

int ConnectToServer(int sd, char* host, int port)
{
    struct sockaddr_in server;
    struct hostent	*hp;
    bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if ((hp = gethostbyname(host)) == NULL)
	{
		return -1;
	}
	bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);

	// Connecting to the server
	if (connect (sd, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		return -1;
	}

    return 1;
}

int WaitForPacket(int sd, char* rBuf )
{
    char* bp = rBuf;
    int bytes_to_read = BUFLEN, n = 0;
    while ((n = recv (sd, bp, bytes_to_read, 0)) < BUFLEN)
	{
        if(n < 0)
            return -1;
		bp += n;
		bytes_to_read -= n;
	}
    return BUFLEN;
}

void* RecvThreadFunc(void* ptr)
{
    char rBuf[BUFLEN];
    int sd = *(int*)ptr, ret = 0;
    while((ret = WaitForPacket(sd, rBuf)) > 0)
    {
        printf("%s", rBuf);
        fflush(stdout);
    }
    printf("Connection ended\n");
    return NULL;
}

void* SendThreadFunc(void* ptr)
{
    char sBuf[BUFLEN];
    int sd = *(int*)ptr, ret = 0;
    while(fgets(sBuf,BUFLEN,stdin))
    {
        ret = send (sd, sBuf, BUFLEN, 0);
        if (ret < 0)
        {
            break;
        }
        printf("%10s%s", "sent:", sBuf);
    }
    return NULL;
}

void* RecvThreadFuncSave(void* ptr)
{
    char rBuf[BUFLEN];
    SaveArg* arg = (SaveArg*) ptr;
    int sd = arg->sd, ret = 0;
    while((ret = WaitForPacket(sd, rBuf)) > 0)
    {
        printf("%s", rBuf);
        fflush(stdout);
        fprintf(arg->fp,"%s", rBuf);

    }
    printf("Connection ended\n");
    return NULL;
}

void* SendThreadFuncSave(void* ptr)
{
    char sBuf[BUFLEN];
    SaveArg* arg = (SaveArg*) ptr;
    int sd = arg->sd, ret = 0;
    while(fgets(sBuf,BUFLEN,stdin))
    {
        ret = send (sd, sBuf, BUFLEN, 0);
        if (ret < 0)
        {
            break;
        }
        printf("%10s%s", "sent: ", sBuf);
        fprintf(arg->fp,"%10s%s", "sent: ", sBuf);
    }
    return NULL;
}

