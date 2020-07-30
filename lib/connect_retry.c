#include "tool.h"

/*
 * Try to connect with exponential backoff.
 */
int connect_retry(
    int domain,
    int type,
    int protocol,
    const struct sockaddr *addr,
    socklen_t alen)
{
    int numsec, fd;
    for (numsec = 1; numsec <= MAXSLEEP; numsec <<= 1)
    {
        if ((fd = socket(domain, type, protocol)) < 0)
            return -1;

        if (connect(fd, addr, alen) == 0)
            /* Connection accepted. */
            return fd;

        close(fd);
        /* Delay before trying again. */
        if (numsec <= MAXSLEEP / 2)
            sleep(numsec);
    }

    return -1;
}