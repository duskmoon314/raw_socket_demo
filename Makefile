build: server.c
	gcc server.c -o server

clean:
	rm server

.PHONY: clean