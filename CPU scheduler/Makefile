CC = g++
CC1 = clang

CFLAGS = -Wall -Wpedantic -Wextra -Werror -lpthread

all: scheduler semaphor

scheduler:	scheduler.cpp
	$(CC) $(CFLAGS) scheduler.cpp -o scheduler
 
semaphor:	semaphor.c
	$(CC1) $(CFLAGS) semaphor.c -o semaphor 

clean:
	rm -f scheduler semaphor



