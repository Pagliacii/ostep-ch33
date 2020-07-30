#include "tool.h"
#include <errno.h>
#include <sys/socket.h>

#define BUFLEN 128

void serve(int sockfd)
{
    int clfd;
    FILE *fp;
    char buf[BUFLEN];

    while (1)
    {
        clfd = accept(sockfd, NULL, NULL);
        guard(clfd >= 0, "accept failed");

        if ((fp = popen("/bin/date", "r")) == NULL)
        {
            sprintf(buf, "error: %s\n", strerror(errno));
            send(clfd, buf, strlen(buf), 0);
        }
        else
        {
            while (fgets(buf, BUFLEN, fp) != NULL)
                send(clfd, buf, strlen(buf), 0);
            pclose(fp);
        }

        close(clfd);
    }
}
