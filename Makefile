all: agent

.PHONY: all

agent: create_and_bind.o listener.o agent.o
	@gcc -o agent create_and_bind.o listener.o agent.o

clean:
	@rm -f agent *.o
