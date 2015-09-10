CC = gcc
#CCFLAGS += -g -Wall -pedantic -Wextra -std=c99 -D_POSIX_SOURCE
CCFLAGS += -g -Wall -pedantic -Wextra -std=c99  -D_POSIX_C_SOURCE=200112L
LDFLAGS += -lz -lncurses -lpng
OBJECTS = server.o network.o ncurses.o file.o

netpaint-server: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) -o netpaint-server $(LDFLAGS)

server.o: server.c server.h
	$(CC) -c server.c $(CCFLAGS)

network.o: network.c network.h
	$(CC) -c network.c $(CCFLAGS)

ncurses.o: ncurses.c ncurses.h
	$(CC) -c ncurses.c $(CCFLAGS)

file.o: file.c file.h
	$(CC) -c file.c $(CCFLAGS)

clean:
	@ \rm -rf *.o core netpaint-server

