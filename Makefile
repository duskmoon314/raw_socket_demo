opt: opt_server.c opt_client.c
	gcc opt_server.c utils.c -o dist/opt_server
	gcc opt_client.c utils.c -o dist/opt_client

dtp: dtp_server.c dtp_client.c
	gcc dtp_server.c utils.c dtp_config.c -o dist/dtp_server
	gcc dtp_client.c utils.c dtp_config.c -o dist/dtp_client

dtp6: dtp_server6.c dtp_client6.c
	gcc dtp_server6.c utils.c dtp_config.c -o dist/dtp_server6
	gcc dtp_client6.c utils.c dtp_config.c -o dist/dtp_client6

run_opt: opt
	tmux new-session -d "tshark -i loopback -T fields -e udp -e ip.dsfield.dscp 'udp port 9000'" && \
	tmux set-option remain-on-exit on && \
	tmux split-window -h "dist/opt_server" && \
	tmux split-window "dist/opt_client" && \
	tmux -2 attach-session -d

run_dtp: dtp
	tmux new-session -d "tshark -i loopback -T fields -e udp -e ip.dsfield.dscp 'udp port 9000'" && \
	tmux set-option remain-on-exit on && \
	tmux split-window -h "dist/dtp_server" && \
	tmux split-window "dist/dtp_client" && \
	tmux -2 attach-session -d

gen: gen_trace.py
	python3 gen_trace.py

test: dtp
	dist/dtp_server > logs/server.log & dist/dtp_client > logs/client.log

clean:
	rm dsit/*

.PHONY: clean