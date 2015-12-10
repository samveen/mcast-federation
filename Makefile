CFLAGS=-std=gnu99 -Wall -Wextra

LDFLAGS=-lcurl

all: agent server client

.PHONY: all

agent: agent.o create_and_bind.o listener_epoll.o listener_no_epoll.o message_impl.o publisher.o peer_info.o
	gcc -o agent agent.o create_and_bind.o message_impl.o listener_no_epoll.o $(LDFLAGS)

server: server.o create_and_bind.o listener_epoll.o listener_no_epoll.o message_impl.o publisher.o peer_info.o
	gcc -o server server.o create_and_bind.o message_impl.o listener_no_epoll.o $(LDFLAGS)

client: client.o create_and_bind.o listener_epoll.o listener_no_epoll.o message_impl.o publisher.o peer_info.o
	gcc -o client client.o publisher.o message_impl.o $(LDFLAGS)

clean:
	@rm -f agent server client *.o
