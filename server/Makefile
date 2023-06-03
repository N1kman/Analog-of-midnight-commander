CC = gcc
CFLAGS = -W -Wall -pthread -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic
.PHONY: clean

all: server
program: server.c Makefile
	$(CC) $(CFLAGS) server.c -o server
clean:
	rm -f server
