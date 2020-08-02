#include "tool.h"
#include <aio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#define BUFLEN 128

/* for tracking I/O requests */
struct ioRequest
{
    int clfd;
    int status;
    struct aiocb *aiocbp;
};

int parse_message(char *message, char *filename)
{
    const char *prefix = "/file/";
    int length;

    if (strncmp(prefix, message, strlen(prefix)) != 0)
        return -1;

    length = strlen(message) - strlen(prefix);
    memset(filename, 0, length + 1);
    memcpy(filename, &message[strlen(prefix)], length);
    return 0;
}

void serve(int sockfd)
{
    fd_set active_fd_set, read_fd_set;
    struct ioRequest *ioList;
    struct aiocb *aiocbList;
    int fd, dir, i, j, rc;
    int numReqs;  /* Total number of queued I/O requests */
    int openReqs; /* Number of I/O requests still in progress */
    char buf[BUFLEN], fname[BUFLEN];
    struct stat *st;
    struct timeval timeout;

    /* Allocate our arrays */
    ioList = calloc(MAXREQS, sizeof(struct ioRequest));
    guard(ioList != NULL, "calloc failed");
    aiocbList = calloc(MAXREQS, sizeof(struct aiocb));
    guard(aiocbList != NULL, "calloc failed");

    /* Initialize the set of active sockets. */
    FD_ZERO(&active_fd_set);
    FD_SET(sockfd, &active_fd_set);

    /* Initialize the counters */
    numReqs = 0;
    openReqs = 0;

    /* Initialize the timeout */
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    while (1)
    {
        read_fd_set = active_fd_set;
        rc = select(FD_SETSIZE, &read_fd_set, NULL, NULL, &timeout);
        guard(rc >= 0, "select failed");

        /* Service all the sockets */
        for (i = 0; i < FD_SETSIZE; i++)
        {
            if (!FD_ISSET(i, &read_fd_set))
                continue;

            if (i == sockfd)
            {
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
                continue;
            }

            /* recvice filename from the connectted socket. */
            memset(buf, 0, BUFLEN);
            memset(fname, 0, BUFLEN);
            rc = recv(i, buf, BUFLEN, 0);
            guard(rc != -1, "recv failed");
            fprintf(stderr, "Server: got message '%s'\n", buf);
            rc = parse_message(buf, fname);
            if (rc != 0)
            {
                sprintf(buf, "error: parse failed\n");
                send(i, buf, strlen(buf), 0);
                close(i);
                continue;
            }
            fprintf(stderr, "Server: parse filename '%s'\n", fname);

            /* get file descriptor */
            dir = open("./files", O_RDONLY | O_DIRECTORY);
            guard(dir != -1, "open failed");
            fd = openat(dir, fname, O_RDONLY);
            guard(fd != -1, "openat failed");
            st = malloc(sizeof(struct stat));
            guard(st != NULL, "malloc failed");
            rc = fstat(fd, st);
            guard(rc == 0, "fstat failed");

            ioList[numReqs].clfd = i;
            ioList[numReqs].status = EINPROGRESS;
            ioList[numReqs].aiocbp = &aiocbList[numReqs];

            /* fill the aiocbp structure */
            ioList[numReqs].aiocbp->aio_fildes = fd;
            ioList[numReqs].aiocbp->aio_nbytes = st->st_size;
            guard(
                (ioList[numReqs].aiocbp->aio_buf = malloc(st->st_size)) != NULL,
                "malloc failed");
            ioList[numReqs].aiocbp->aio_reqprio = 0;
            ioList[numReqs].aiocbp->aio_offset = 0;

            rc = aio_read(ioList[numReqs].aiocbp);
            guard(rc != -1, "aio_read failed");
            numReqs++;
        }

        openReqs = numReqs;

        /* Loop, monitoring status of I/O requests */
        if (openReqs > 0)
        {
            for (j = 0; j < numReqs; j++)
            {
                if (ioList[j].status != EINPROGRESS)
                    continue;

                /* update status */
                ioList[j].status = aio_error(ioList[j].aiocbp);

                if (ioList[j].status == 0)
                {
                    /* async I/O succeeded */
                    fprintf(stderr, "Server: I/O succeeded\n");
                    rc = send(ioList[j].clfd, (char *)ioList[j].aiocbp->aio_buf, ioList[j].aiocbp->aio_nbytes, 0);
                    guard(rc != -1, "send failed");
                    close(ioList[j].aiocbp->aio_fildes);
                    close(ioList[j].clfd);
                    FD_CLR(ioList[j].clfd, &active_fd_set);
                }

                if (ioList[j].status != EINPROGRESS)
                    openReqs--;
            }
        }

        if (openReqs == 0 && numReqs != 0)
        {
            /* all I/O requests completed */
            fprintf(stderr, "Server: all I/O requests completed\n");
            close(sockfd);
            exit(0);
        }
    }

    exit(0);
}