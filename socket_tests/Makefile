CC = gcc
CTARGET = clientTCP clientUDP clientIPinIP clientRAW clientTCPRAW serverIPinIP serverRAW serverUDP serverTCP clientIPinIP2

%: %.c
	$(CC) -o $@ $<

clean:
	rm -f $(CTARGET)
