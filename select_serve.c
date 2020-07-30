#include "tool.h"
#include <arpa/inet.h>
#include <sys/socket.h>

extern void process(int);

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
