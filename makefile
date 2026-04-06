CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SRC = main.c BFS.c
HDR = BFS.h

all: main

main: $(SRC) $(HDR)
	$(CC) $(CFLAGS) -o main $(SRC) -I.

clean:
	rm -f main main.exe
