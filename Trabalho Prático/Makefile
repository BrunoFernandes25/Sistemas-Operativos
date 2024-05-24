CC = gcc
CFLAGS = -Wall -g -Iinclude
LDFLAGS =
all: folders orchestrator client hello void
orchestrator: bin/orchestrator
client: bin/client
hello: bin/hello
void: bin/void

folders:
	@mkdir -p src include obj bin tmp
bin/orchestrator: obj/orchestrator.o
	$(CC) $(LDFLAGS) $^ -o $@
bin/client: obj/client.o
	$(CC) $(LDFLAGS) $^ -o $@
bin/hello: obj/hello.o
	$(CC) $(LDFLAGS) $^ -o $@
bin/void: obj/void.o
	$(CC) $(LDFLAGS) $^ -o $@	
obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f output/* obj/* tmp/* bin/*
