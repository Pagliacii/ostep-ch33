#include "tool.h"
#include <errno.h>

#define BUFLEN 128

void process(int sockfd)
{
    char buf[BUFLEN], msg[BUFLEN], filename[BUFLEN];
    /* FILE *fp; */
    int nbytes;
    char *prefix = "/file/";

    nbytes = read(sockfd, buf, BUFLEN);
    guard(nbytes >= 0, "read failed");
    if (nbytes == 0)
    {
        /* End-of-file */
        close(sockfd);
    }

    strncpy(msg, buf, nbytes);
    msg[nbytes] = '\0';
    fprintf(stderr, "Server: got message: '%s'\n", msg);

    if (strncmp(prefix, msg, strlen(prefix)) != 0)
    {
        sprintf(buf, "error: unrecognizable message '%s'\n", msg);
        send(sockfd, buf, strlen(buf), 0);
    }
    else
    {
        int fnlen = nbytes - strlen(prefix);
        memcpy(filename, &msg[strlen(prefix)], fnlen);
        filename[fnlen] = '\0';
        fprintf(stderr, "Server: got filename '%s'\n", filename);
    }

    close(sockfd);
}