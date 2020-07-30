#include "tool.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
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

void serve(int sockfd)
{
    fd_set active_fd_set, read_fd_set;
    int i, rc;

    /* Initialize the set of active sockets. */
    FD_ZERO(&active_fd_set);
    FD_SET(sockfd, &active_fd_set);

    while (1)
    {
        /* Block until input arrives on one or more active sockets. */
        read_fd_set = active_fd_set;
        rc = select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL);
        guard(rc >= 0, "select failed");

        /* Service all the sockets with input pending. */
        for (i = 0; i < FD_SETSIZE; i++)
        {
            if (!FD_ISSET(i, &read_fd_set))
                continue;

            if (i != sockfd)
            {
                process(i);
                FD_CLR(i, &active_fd_set);
                continue;
            }

            int size, clfd;
            struct sockaddr_in peer_addr;
            size = sizeof(peer_addr);
            clfd = accept(sockfd, (struct sockaddr *)&peer_addr, (socklen_t *)&size);
            guard(clfd >= 0, "accept failed");

            fprintf(
                stderr,
                "Server: connect from host %s, port %hd.\n",
                inet_ntoa(peer_addr.sin_addr),
                ntohs(peer_addr.sin_port));
            FD_SET(clfd, &active_fd_set);
        }
    }
}
