CFLAGS=-std=gnu99

LDFLAGS=-lcurl

all: agent

.PHONY: all

agent: create_and_bind.o listener.o agent.o build_message.o publisher.o
	@gcc -o agent create_and_bind.o listener.o agent.o $(LDFLAGS)

clean:
	@rm -f agent *.o
