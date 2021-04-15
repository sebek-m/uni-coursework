CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -O2
STRUC=structure.h
HDRS1=utils.h declare.h valid.h remove.h energy.h equal.h
HDRS2=utils.h relation-utils.h tree-search.h $(STRUC)
OBJS=$(HDRS1:.h=.o) relation-utils.o tree-search.o pre-post.o cmd-reader.o structure.o

all: quantization

quantization: main.o $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

utils.o: utils.c utils.h
	$(CC) -c $< $(CFLAGS)

structure.o: structure.c structure.h
	$(CC) -c $< $(CFLAGS)

relation-utils.o: relation-utils.c relation-utils.h $(STRUC)
	$(CC) -c $< $(CFLAGS)

tree-search.o: tree-search.c tree-search.h $(STRUC)
	$(CC) -c $< $(CFLAGS)

declare.o: declare.c declare.h utils.h
	$(CC) -c $< $(CFLAGS)

valid.o: valid.c valid.h utils.h tree-search.h $(STRUC)
	$(CC) -c $< $(CFLAGS)

remove.o: remove.c remove.h $(HDRS2)
	$(CC) -c $< $(CFLAGS)

energy.o: energy.c energy.h utils.h tree-search.h $(STRUC)
	$(CC) -c $< $(CFLAGS)

equal.o: equal.c equal.h $(HDRS2)
	$(CC) -c $< $(CFLAGS)

pre-post.o: pre-post.c pre-post.h $(STRUC)
	$(CC) -c $< $(CFLAGS)

cmd-reader.o: cmd-reader.c cmd-reader.h $(HDRS1) cmd-info.h
	$(CC) -c $< $(CFLAGS)

main.o: main.c $(STRUC) pre-post.h cmd-reader.h
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f *.o quantization
