#include "server.h"

#define PORT_NO         9999
#define BUFLEN          128
#define MAX_NUM_CLIENTS 16

int main()
{
    int32_t listen_sd;

    if ((listen_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        std::cerr << strerror(errno) << std::endl;
    }

    
}
