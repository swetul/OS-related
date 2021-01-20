CC = clang
CFLAGS = -Wall -Wpedantic -Wextra -Werror -g -lm

all: allFiles

allFiles:	main.c Host.c
	$(CC) $(CFLAGS) main.c Host.c physical.o -o main

clean:
	rm -f main
