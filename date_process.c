#include "tool.h"
#include <errno.h>
#include <sys/socket.h>

#define BUFLEN 128

void process(int sockfd)
{
    char buf[BUFLEN], msg[BUFLEN];
    FILE *fp;
    int nbytes;

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

    if ((fp = popen("/bin/date", "r")) == NULL)
    {
        sprintf(buf, "error: %s\n", strerror(errno));
        send(sockfd, buf, strlen(buf), 0);
    }
    else
    {
        while (fgets(buf, BUFLEN, fp) != NULL)
            send(sockfd, buf, strlen(buf), 0);
        pclose(fp);
    }
    close(sockfd);
}