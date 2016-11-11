# build an executable from socket_server.c

CC = gcc
CFLAGS = -I.
DEPS = print_packet.h
CTARGET = socket_server

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

socket_server: socket_server.o print_packet.o
	$(CC) -o socket_server socket_server.o print_packet.o -I.

clean:
	rm -f $(CTARGET)
	rm *.o
