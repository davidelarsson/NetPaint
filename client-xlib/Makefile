
# Kompilator
CC = gcc

# Kompilatorflaggor, l�gg till '-g' om kompilering f�r avlusning ska g�ras.
#CCFLAGS += -pedantic -Wall -Wextra -std=c99 -D_POSIX_SOURCE -std=c99 -g
CCFLAGS += -pedantic -Wall -Wextra -std=c99 -std=c99 -g -D_POSIX_C_SOURCE=200112L

# L�nkflaggor - Fix f�r att l�sa lokala problem med C++ l�nkbibliotek.
LDFLAGS += -lX11 -lm -lz -lpng

# Objektkodsmoduler som ing�r i den kompletta kalkylatorn.
OBJECTS = network.o netpaint.o xlib.o tool.o tool_line.o tool_rectangle.o tool_freehand.o tool_circle.o tool_text.o file.o

# Huvudm�l - skapas med kommandot 'make'
netpaint-client: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) -o netpaint-client $(LDFLAGS)

# Delm�l (flaggan -c avbryter innan l�nkning, objektkodsfil erh�lls)
network.o: network.c network.h
	$(CC) -c network.c $(CCFLAGS)

xlib.o: xlib.c xlib.h
	$(CC) -c xlib.c $(CCFLAGS)

tool.o: tool.c tool.h
	$(CC) -c tool.c $(CCFLAGS)

tool_line.o: tool_line.c tool_line.h
	$(CC) -c tool_line.c $(CCFLAGS)

tool_rectangle.o: tool_rectangle.c tool_rectangle.h
	$(CC) -c tool_rectangle.c $(CCFLAGS)

tool_freehand.o: tool_freehand.c tool_freehand.h
	$(CC) -c tool_freehand.c $(CCFLAGS)

tool_circle.o: tool_circle.c tool_circle.h
	$(CC) -c tool_circle.c $(CCFLAGS)

tool_text.o: tool_text.c tool_text.h
	$(CC) -c tool_text.c $(CCFLAGS)

file.o: file.c file.h
	$(CC) -c file.c $(CCFLAGS)

netpaint.o: netpaint.c netpaint.h
	$(CC) -c netpaint.c $(CCFLAGS)

# 'make clean' tar bort objektkodsfiler och 'core' (minnesdump).
clean:
	@ \rm -rf *.o core netpaint-client

