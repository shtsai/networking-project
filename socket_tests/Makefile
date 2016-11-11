CC = gcc
CTARGET = clientTCP clientUDP clientIPinIP clientRAW clientTCPRAW

%: %.c
	$(CC) -o $@ $<

clean:
	rm -f $(CTARGET)
