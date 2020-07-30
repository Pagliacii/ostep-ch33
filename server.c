#include "tool.h"
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 8080

extern void serve(int);

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sockfd, backlog = 1;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    size_t addrlen = sizeof(address);

    sockfd = init_server(
        SOCK_STREAM,
        (struct sockaddr *)&address,
        (socklen_t *)&addrlen,
        backlog);
    serve(sockfd);

    exit(EXIT_SUCCESS);
}