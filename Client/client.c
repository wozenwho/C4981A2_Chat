/*------------------------------------------------------------------------------
--	Source File:	client.c
--
--	Program:		Chat Client
--
--  Functions       
--                  int CreateSock();
--                  int ConnectToServer(int sd, char* host, int port);
--                  int WaitForPacket(int sd, char* rBuf );
--                  void* RecvThreadFunc(void* ptr);
--                  void* SendThreadFunc(void* ptr);
--                  void* RecvThreadFuncSave(void* ptr);
--                  void* SendThreadFuncSave(void* ptr);
--
--	Date:			Apr 4 2018
--
--	Designer:		Jeffrey Chou
--
--	Programmer:		Jeffrey Chou
--
--	Notes:
--  Chat client that connects to a server passed in via command line arguments.
--  Creates a thread for receiving from the server and for getting input from the
--  user. 
--
--	Contains client thread functions and TCP wrapper functions
--
-------------------------------------------------------------------------------*/

#include "client.h"

/*------------------------------------------------------------------------------
--	Function:		CreateSock
--
--	Date:			Apr 4 2018
--
--	Designer:		Jeffrey Chou
--
--	Programmer:		Jeffrey Chou
--
--	Interface:		int CreateSock()
--
--	Returns:		-1 if function failed, else returns socket descriptor
--
--	Notes:
--	Initializes a socket that will be used for TCP connections
--
------------------------------------------------------------------------------*/
int CreateSock()
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

/*------------------------------------------------------------------------------
--	Function:		ConnectToServer
--
--	Date:			Apr 4 2018
--
--	Designer:		Jeffrey Chou
--
--	Programmer:		Jeffrey Chou
--
--	Interface:		int ConnectToServer(int sd, char* host, int port)
--                       int sd: socket descriptor that was initialized
--                       char* host: host ip address
--                       int port: port number server is listening on
--
--	Returns:		-1 if connection could not be established, 1 if connection
--                  succeeds
--
--	Notes:
--	Initiates a connection on a socket
--
------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------
--	Function:		WaitForPacket
--
--	Date:			Apr 4 2018
--
--	Designer:		Jeffrey Chou
--
--	Programmer:		Jeffrey Chou
--
--	Interface:		int WaitForPacket(int sd, char* rBuf )
--                       int sd: socket descriptor that is connected to server
--                       char* rBuf: pointer to the receive buffer
--
--	Returns:	    Returns the size of the packet on completion or -1 if an 
--                  error occurs
--
--	Notes:
--	Continually calls recv until the expected packet size is read
--
------------------------------------------------------------------------------*/
int WaitForPacket(int sd, char* rBuf )
{
    char* bp = rBuf;
    int bytes_to_read = BUFLEN, n = 0;
    while ((n = recv (sd, bp, bytes_to_read, 0)) < BUFLEN)
	{
        if(n <= 0)
            return -1;
		bp += n;
		bytes_to_read -= n;
	}
    return BUFLEN;
}

/*------------------------------------------------------------------------------
--	Function:		RecvThreadFunc
--
--	Date:			Apr 4 2018
--
--	Designer:		Jeffrey Chou
--
--	Programmer:		Jeffrey Chou
--
--	Interface:		void* RecvThreadFunc(void* ptr )
--                       void* ptr: parameter passed to thread func. is the socket
--                          descriptor for this function
--
--	Returns:	    Returns NULL on completion
--
--	Notes:
--	Thread function used to receive tcp packets from server and display messages
--  onto the screen
--
------------------------------------------------------------------------------*/
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
    close(sd);
    return NULL;
}

/*------------------------------------------------------------------------------
--	Function:		SendThreadFunc
--
--	Date:			Apr 4 2018
--
--	Designer:		Jeffrey Chou
--
--	Programmer:		Jeffrey Chou
--
--	Interface:		void* SendThreadFunc(void* ptr )
--                       void* ptr: parameter passed to thread func. is the socket
--                          descriptor for this function
--
--	Returns:	    Returns NULL on completion
--
--	Notes:
--	Thread function used to get user input and it off to the server
--
------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------
--	Function:		RecvThreadFuncSave
--
--	Date:			Apr 4 2018
--
--	Designer:		Jeffrey Chou
--
--	Programmer:		Jeffrey Chou
--
--	Interface:		void* RecvThreadFunc(void* ptr )
--                       void* ptr: parameter passed to thread func, which is a structure
--                          that contains the socket descriptor and a pointer to the file
--                          to write to
--
--	Returns:	    Returns NULL on completion
--
--	Notes:
--	Similar to RecvThreadFunc but also writes messages onto a file
--
------------------------------------------------------------------------------*/
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
    close(sd);
    return NULL;
}

/*------------------------------------------------------------------------------
--	Function:		SendThreadFuncSave
--
--	Date:			Apr 4 2018
--
--	Designer:		Jeffrey Chou
--
--	Programmer:		Jeffrey Chou
--
--	Interface:		void* SendThreadFunc(void* ptr )
--                       void* ptr: parameter passed to thread func, which is a structure
--                          that contains the socket descriptor and a pointer to the file
--                          to write to
--
--	Returns:	    Returns NULL on completion
--
--	Notes:
--	Similar to SendThreadFunc but also writes messages onto a file
--
------------------------------------------------------------------------------*/
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

