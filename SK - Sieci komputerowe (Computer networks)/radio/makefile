CC = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++17
TARGETS = radio-proxy radio-client

all: $(TARGETS)

err.o: err.h

utils.o: utils.h

client-info-class.o: client-info-class.h utils.h exceptions.h

client-handler-class.o: client-handler-class.h client-info-class.h utils.h

outputter-class.o: outputter-class.h client-handler-class.h utils.h

processor-class.o: processor-class.h outputter-class.h utils.h

proxy-class.o: proxy-class.h processor-class.h client-handler-class.h utils.h exceptions.h

proxy-initializer.o: proxy-initializer.h proxy-class.h client-handler-class.h \
										 outputter-class.h processor-class.h utils.h exceptions.h

radio-info-class.o: radio-info-class.h utils.h

radio-client-class.o: radio-client-class.h utils.h exceptions.h

radio-proxy: radio-proxy.o proxy-initializer.o proxy-class.o processor-class.o \
						 outputter-class.o client-handler-class.o client-info-class.o utils.o err.o
	$(CC) $(CXXFLAGS) $^ -o $@

radio-client: radio-client.o radio-client-class.o radio-info-class.o utils.o err.o
	$(CC) $(CXXFLAGS) $^ -o $@ -lpthread

clean:
	rm -f *.o $(TARGETS) 
