CC = gcc
CTARGET = clientTCP clientUDP clientIPinIP clientRAW clientTCPRAW serverIPinIP serverRAW serverUDP serverTCP

%: %.c
	$(CC) -o $@ $<

clean:
	rm -f $(CTARGET)
