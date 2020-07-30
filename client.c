#include "tool.h"
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFLEN 128
#define PORT 8080

int main(int argc, char const *argv[])
{
    int sockfd, n, rc;
    struct sockaddr_in serv_addr;
    int addrlen = sizeof(serv_addr);
    char buf[BUFLEN];
    char *hello = "Hello from client";

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    rc = inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    guard(rc > 0, "invalid address");
    sockfd = connect_retry(AF_INET, SOCK_STREAM, 0, (struct sockaddr *)&serv_addr, addrlen);
    guard(sockfd >= 0, "connection failed");

    send(sockfd, hello, strlen(hello), 0);
    fprintf(stderr, "Hello message sent\n");
    while ((n = recv(sockfd, buf, BUFLEN, 0)) > 0)
        write(STDOUT_FILENO, buf, n);
    guard(n >= 0, "recv error");

    exit(EXIT_SUCCESS);
}
