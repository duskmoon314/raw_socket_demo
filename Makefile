build: opt_server.c opt_client.c
	gcc opt_server.c utils.c -o dist/opt_server
	gcc opt_client.c utils.c -o dist/opt_client

run: build
	tmux new-session -d "tshark -i loopback -T fields -e udp -e ip.dsfield.dscp 'udp port 9000'" && \
	tmux set-option remain-on-exit on && \
	tmux split-window -h "dist/opt_server" && \
	tmux split-window "dist/opt_client" && \
	tmux -2 attach-session -d

clean:
	rm dsit/*

.PHONY: clean