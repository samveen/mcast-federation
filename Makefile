CFLAGS=-std=gnu99 -Wall -Wextra

LDFLAGS=-lcurl

all: agent

.PHONY: all

agent: create_and_bind.o listener.o agent.o build_message.o publisher.o peer_info.o listener_no_epoll.o
	@gcc -o agent create_and_bind.o listener.o agent.o $(LDFLAGS)

clean:
	@rm -f agent *.o
