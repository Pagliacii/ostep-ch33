ROOT = ..
include $(ROOT)/Make.defines

LIBMISC = libtool.a
SRCS    = connect_retry.c guard.c init_server.c
OBJS    = ${SRCS:c=o}

.PHONY: all clean
all: $(LIBMISC)

%.o : %.c
> $(CC) $(CFLAGS) -c $<

$(LIBMISC): $(OBJS)
> $(AR) rv $(LIBMISC) $?

clean:
> rm -f $(TEMPFILES) $(LIBMISC)