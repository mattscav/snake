CC = gcc

CFLAGS = $(shell pkg-config --cflags sdl2)
LDFLAGS = $(shell pkg-config --libs sdl2) 

snake: main.c
	$(CC) main.c -o snake $(CFLAGS) $(LDFLAGS)

run: 
	./snake
