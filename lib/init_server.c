#include "tool.h"

#include <errno.h>

int init_server(
    int type,
    const struct sockaddr *addr,
    socklen_t *alen,
    int backlog)
{
    int err, fd;
    int reuse = 1;

    if ((fd = socket(addr->sa_family, type, 0)) < 0)
        return -1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &reuse, sizeof(int)) < 0)
        goto errout;
    if (bind(fd, addr, *alen) < 0)
        goto errout;
    if (type == SOCK_STREAM || type == SOCK_SEQPACKET)
    {
        if (listen(fd, backlog) < 0)
            goto errout;
    }
    return fd;

errout:
    err = errno;
    close(fd);
    errno = err;
    return -1;
}