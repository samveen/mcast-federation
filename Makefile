CFLAGS=-std=gnu99 -Wall -Wextra

LDFLAGS=-lcurl

all: agent

.PHONY: all

agent: create_and_bind.o listener_epoll.o listener_no_epoll.o agent.o message_impl.o publisher.o peer_info.o
	@gcc -o agent create_and_bind.o message_impl.o listener_no_epoll.o agent.o $(LDFLAGS)

clean:
	@rm -f agent *.o
