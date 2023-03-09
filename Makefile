CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=server.cpp client.cpp util.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLES=server client

all: $(SOURCES) $(EXECUTABLES)

server: server.o util.o
	$(CC) $(LDFLAGS) server.o util.o -o server

client: client.o util.o
	$(CC) $(LDFLAGS) client.o util.o -o client

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLES)
