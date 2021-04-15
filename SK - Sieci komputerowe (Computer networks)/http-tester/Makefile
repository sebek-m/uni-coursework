CC = gcc
CFLAGS = -Wall
TARGETS = testhttp_raw

all: $(TARGETS) 

err.o: err.c err.h

utils.o: utils.c utils.h err.h

parser.o: parser.c parser.h utils.h err.h

msg_builder.o: msg_builder.c msg_builder.h parser.h err.h

connector.o: connector.c connector.h utils.h err.h

response_parser.o: response_parser.c response_parser.h utils.h err.h

testhttp_raw.o: testhttp_raw.c err.h utils.h parser.h msg_builder.h connector.h response_parser.h

testhttp_raw: testhttp_raw.o err.o utils.o parser.o msg_builder.o connector.o response_parser.o

clean:
	rm -f *.o $(TARGETS) 
