CFLAGS=-std=gnu99 -Wall -Wextra -Wno-unused-parameter

LDFLAGS=-lcurl

all: agent server client

.PHONY: all

server: server.o create_and_bind.o listener_no_epoll.o message_impl.o publisher.o peer_info.o trigger_waiting_room.o curl_data.o
	gcc -o server $^ $(LDFLAGS)

client: client.o publisher.o message_impl.o peer_info.o curl_data.o
	gcc -o client $^ $(LDFLAGS)

agent: agent.o create_and_bind.o listener_no_epoll.o message_impl.o publisher.o peer_info.o trigger_waiting_room.o curl_data.o options.o
	gcc -o agent $^ $(LDFLAGS)

clean:
	@rm -f agent server client *.o
