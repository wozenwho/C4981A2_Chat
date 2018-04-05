/**
 * SOURCE FILE: server.cpp
 * 
 * PROGRAM:     server
 * 
 * FUNCTIONS:   main()
 *              transmitMessage(int sockfd, size_t index, char* buffer)
 * 
 * DATE:        April 2nd, 2018
 * 
 * REVISIONS:   
 * 
 * DESIGNER:    Wilson Hu
 * 
 * PROGRAMMER:  Wilson Hu
 * 
 * NOTES:       
 * This is a server program that handles multiple clients via the SELECT system call.
 * It acts as a chat server that allows client programs to connect and communicate
 * via text to other connected client programs.
 */
#include "server.h"

size_t  numClients = 0;
int32_t clientArr[MAX_NUM_CLIENTS];
struct sockaddr_in* sockaddrArr[MAX_NUM_CLIENTS];

int32_t maxi;


/**
 * FUNCTION:    transmitMessage
 * 
 * DATE:        April 2nd, 2018
 * 
 * REVISIONS:
 * 
 * DESIGNER:    Wilson Hu
 * 
 * PROGRAMMER:  Wilson Hu
 * 
 * INTERFACE:   int main()
 * 
 * RETURNS:     int - returns 0 on normal program termination
 * 
 * NOTES:
 * This function is the entry point into the program. 
 * 
 * It contains the loop which holds the SELECT and connection logic of the program.
 * It calls transmitMessage when it receives a message from a client.
 * 
 * It adds each connected cilent to an array of sockaddr_in struct pointers which
 * is used to store the IP addresses of each connected client.
 */
int main()
{
    int32_t listen_sd;
    int32_t result;
    int32_t running = 0;
    int32_t arg;
    int32_t maxfd;
    int32_t numReady;
    int32_t sockfd;
    int32_t bytesToRead;
    int32_t i;


    int newClient;


    char* bufferPtr;
    char buffer[BUFLEN];
    int numRead;

    struct  sockaddr_in server;
    struct  sockaddr_in client_addr;
    socklen_t  client_len;

    fd_set  rset;
    fd_set  allset;

    // Clear memory of sockaddr_in structs
    memset(&server, 0, sizeof(struct sockaddr_in));
    memset(&client_addr, 0, sizeof(struct sockaddr_in));

    memset(sockaddrArr, 0, MAX_NUM_CLIENTS * sizeof(sockaddr_in*));

    if ((listen_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        std::cerr << strerror(errno) << std::endl;
        exit(1);
    }
    else
    {


        arg = 1;
        result = setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, &arg, sizeof(arg));
        if (result == -1)
        {
            std::cerr << strerror(errno) << std::endl;
            exit(1);
        }

        server.sin_family = AF_INET;
        server.sin_port = htons(PORT_NO);
        server.sin_addr.s_addr = htonl(INADDR_ANY);

        result = bind(listen_sd, (struct sockaddr*) &server, sizeof(server));
        if (result == -1)
        {
            std::cerr << strerror(errno) << std::endl;
            exit(1);
        }

        listen(listen_sd, MAX_NUM_CLIENTS);

        maxfd = listen_sd;
        for (i = 0; i < MAX_NUM_CLIENTS; i++)
        {
            clientArr[i] = -1;
        }
        maxi = -1;
        FD_ZERO(&allset);
        FD_SET(listen_sd, &allset);


        running = 1;

        while (running)
        {
            rset = allset;
            numReady = select(maxfd + 1, &rset, NULL, NULL, NULL);

            // Handle new client connectionclientArr
            if (FD_ISSET(listen_sd, &rset))
            {
                client_len = sizeof(struct sockaddr_in);
                newClient = accept(listen_sd, (struct sockaddr*) &client_addr, &client_len);
                if (newClient == -1)
                {
                    std::cerr << strerror(errno) << std::endl;
                    exit(1);
                }
                numClients++;
                std::cerr << "Accepted client: " << inet_ntoa(client_addr.sin_addr) << std::endl;

                for (i = 0; i < FD_SETSIZE; i++)
                    if (clientArr[i] < 0)
                    {
                        clientArr[i] = newClient;
                        sockaddrArr[i] = (struct sockaddr_in*) malloc(sizeof(struct sockaddr_in));
                        memset(sockaddrArr[i], 0, sizeof(struct sockaddr_in));
                        memcpy(sockaddrArr[i], &client_addr, sizeof(struct sockaddr_in));
                        break;
                    }

                if (i == FD_SETSIZE)
                {
                    std::cerr << "Reached max num. clients, exiting." << std::endl;
                }
                // Add new descriptor to set
                FD_SET(newClient, &allset);
                if (newClient > maxfd)
                {
                    maxfd = newClient;
                }
                if (i > maxi)
                {
                    maxi = i;
                }
                if (--numReady <= 0)
                {
                    continue;
                }

            }

            // Check all clients for data
            for (i = 0; i <= maxi; i++)
            {
                if ((sockfd = clientArr[i]) < 0)
                {
                    continue;
                }
                if (FD_ISSET(sockfd, &rset))    // issue here?
                {
                    memset(buffer, '\0', sizeof(buffer));
                    bufferPtr = buffer;
                    bytesToRead = BUFLEN;
                    while ((numRead = read(sockfd, bufferPtr, bytesToRead)) > 0)
                    {
                        bufferPtr += numRead;
                        bytesToRead -= numRead;
                    }
                    // Write to all other clients
                    transmitMessage(sockfd, i, buffer);

                    // Connection has been closed by client
                    if (strlen(buffer) == 0)
                    {
                        std::cerr << "closed client: " << inet_ntoa(sockaddrArr[i]->sin_addr) << std::endl;
                        free(sockaddrArr[i]);
                        sockaddrArr[i] = 0;
                        close(sockfd);
                        FD_CLR(sockfd, &allset);
                        clientArr[i] = -1;
                    }

                    // No more readable descriptors
                    if (--numReady <= 0)
                        break;
                }
            }
        }
    }

    // Free remaining allocated sockaddr_in structs
    for (int i = 0; i < MAX_NUM_CLIENTS; i++)
    {
        if (sockaddrArr[0] != 0)
        {
            free(sockaddrArr[i]);
        }
    }
    return 0;
}

/**
 * FUNCTION:    transmitMessage
 * 
 * DATE:        April 2nd, 2018
 * 
 * REVISIONS:
 * 
 * DESIGNER:    Wilson Hu
 * 
 * PROGRAMMER:  Wilson Hu
 * 
 * INTERFACE:   void transmitMessage(int sockfd, size_t index, char* buffer)
 *                  int sockfd: socket descriptor of the client that sent the message
 *                  size_t index: index of the client in clientArr
 *                  char* buffer: buffer containing contents of the client's message
 * RETURNS:     void
 * 
 * NOTES:
 * This function appends the the IP address of the client that sent the message (sender)
 * and relays the message to all other connected clients.
 */
void transmitMessage(int sockfd, size_t index, char* buffer)
{
    char transmitBuffer[BUFLEN];
    sprintf(transmitBuffer, "%s : %s", inet_ntoa(sockaddrArr[index]->sin_addr), buffer);

    for (size_t i = 0; i <= maxi; i++)
    {
        if (clientArr[i] == sockfd)
        {
            continue;
        }
        write(clientArr[i], transmitBuffer, BUFLEN);
    }
}
