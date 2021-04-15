PROGRAMS = example
CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11

all: $(PROGRAMS) VARIABLES

VARIABLES:
  export N=5

err.o: err.c err.h

notec.o: notec.asm
	nasm -DN=$N -f elf64 -w+all -w+error -o $@ $<

example.o: example.c err.h
	$(CC) -DN=$N -c $(CFLAGS) -o $@ $<

example: example.o notec.o err.o
	$(CC) -pthread -o $@ $^

.PHONY: all clean

clean:
	rm -rf $(PROGRAMS) *.o
