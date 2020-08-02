ROOT = .
include $(ROOT)/Make.defines

PROGS    := client filed selectd simpled aioserver

.PHONY: all clean
all: $(PROGS)

client : client.o $(LIBTOOL) Makefile
> $(CC) $< -o $@ $(LDLIBS)

filed : file_process.o fetch.o select_serve.o server.o $(LIBTOOL) Makefile
> $(CC) $< fetch.o select_serve.o server.o -o $@ $(LDLIBS)

simpled : simple_serve.o server.o $(LIBTOOL) Makefile
> $(CC) $< server.o -o $@ $(LDLIBS)

selectd : date_process.o select_serve.o server.o $(LIBTOOL) Makefile
> $(CC) $< select_serve.o server.o -o $@ $(LDLIBS)

aioserver : aio_serve.o server.o $(LIBTOOL) Makefile
> $(CC) $< server.o -o $@ $(LDLIBS) -lrt

%.o : %.c $(LIBTOOL)
> $(CC) $(CFLAGS) -c $<

clean:
> (cd $(ROOT)/lib && $(MAKE) clean)
> rm -f $(TEMPFILES)
> rm -f $(PROGS)

include $(ROOT)/Make.libtool.inc