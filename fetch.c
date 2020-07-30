#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int fetch_file_contents(int sockfd, const char *filename, const char *dirname)
{
    char *errmsg = "failed to access the file";
    char buf[BUFSIZ];
    int fd, dir, nbytes;

    if (access(dirname, R_OK) < 0)
    {
        fprintf(stderr, "fetch_file_contents: cannot access the directory '%s'\n", dirname);
        goto errout;
    }

    dir = open(dirname, O_RDONLY | O_DIRECTORY);
    if (dir < 0)
    {
        fprintf(stderr, "fetch_file_contens: cannot open the directory '%s'\n", dirname);
        goto errout;
    }

    fd = openat(dir, filename, O_RDONLY);
    if (fd < 0)
    {
        close(dir);
        errmsg = "failed to open the file";
        fprintf(stderr, "fetch_file_contents: %s '%s'\n", errmsg, filename);
        goto errout;
    }

    nbytes = read(fd, buf, BUFSIZ);
    if (nbytes < 0)
    {
        close(fd);
        close(dir);
        errmsg = "failed to read file contents";
        fprintf(stderr, "fetch_file_contents: %s\n", errmsg);
        goto errout;
    }

    send(sockfd, buf, nbytes, 0);
    close(fd);
    close(dir);
    return 0;

errout:
    sprintf(buf, "error: %s", errmsg);
    send(sockfd, buf, strlen(buf), 0);
    return -1;
}