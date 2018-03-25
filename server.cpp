#include "server.h"

#define PORT_NO         9999
#define BUFLEN          256
#define MAX_NUM_CLIENTS 16

void transmitMessage(int sockfd, size_t index, char* buffer);

size_t  numClients = 0;
int32_t clientArr[MAX_NUM_CLIENTS];

int main()
{
    int32_t listen_sd;
    int32_t result;
    int32_t running = 0;
    int32_t arg;
    int32_t maxfd;
    int32_t numReady;
    int32_t maxi;
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

            // Handle new client connection
            if (FD_ISSET(listen_sd, &rset))
            {
                client_len = sizeof(struct sockaddr_in);
                newClient = accept(listen_sd, (struct sockaddr*) &client_addr, &client_len);
                if (newClient == -1)
                {
                    std::cerr << strerror(errno) << std::endl;
                    exit(1);
                }
                std::cerr << "Accepted client: " << inet_ntoa(client_addr.sin_addr) << std::endl;

                for (i = 0; i < FD_SETSIZE; i++)
                    if (clientArr[i] < 0)
                    {
                        clientArr[i] = newClient;
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
                    if (numRead == 0)
                    {

                    }

                    // No more readable descriptors
                    if (--numReady <= 0)
                        break;

                }
            }
        }






    }
    return 0;
}

void transmitMessage(int sockfd, size_t index, char* buffer)
{
    for (size_t i = 0; i < numClients; i++)
    {
        if (i == index)
        {
            continue;
        }
        write(clientArr[i], buffer, BUFLEN);
    }
}
