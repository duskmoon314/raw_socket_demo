build: server.c client.c
	gcc server.c utils.c -o server
	gcc client.c utils.c -o client

clean:
	rm server

.PHONY: clean