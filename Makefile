ROOT = .
include $(ROOT)/Make.defines

SRCS     := client.c
SERVERS  := select_server.c simple_server.c
OBJS     := $(SERVERS:c=o)
PROGS    := client select simple

.PHONY: all clean
all: $(PROGS)

client : client.o $(LIBTOOL) Makefile
> $(CC) $< -o $@ $(LDLIBS)

simple : simple_server.o server.o $(LIBTOOL) Makefile
> $(CC) $< server.o -o $@ $(LDLIBS)

select : select_server.o server.o $(LIBTOOL) Makefile
> $(CC) $< server.o -o $@ $(LDLIBS)

%.o : %.c $(LIBTOOL)
> $(CC) $(CFLAGS) -c $<

clean:
> (cd $(ROOT)/lib && $(MAKE) clean)
> rm -f $(TEMPFILES)
> rm -f $(PROGS)

include $(ROOT)/Make.libtool.inc