#ifndef _TOOL_H
#define _TOOL_H
#include <sys/socket.h> /* for sockaddr and socklen_t */

#include <stdbool.h> /* for bool */
#include <stdio.h>   /* for convenience */
#include <stdlib.h>  /* for convenience */
#include <string.h>  /* for convenience */
#include <unistd.h>  /* for convenience */

#define MAXSLEEP 128

/*
 * Prototypes for our own functions.
 */

void guard(bool, char[]);

int connect_retry(int, int, int, const struct sockaddr *, socklen_t);

int init_server(int, const struct sockaddr *, socklen_t *, int);
#endif