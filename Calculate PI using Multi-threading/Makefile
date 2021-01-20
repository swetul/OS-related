CC = clang
CFLAGS = -Wall -Wpedantic -Wextra -Werror -g -lm -lgmp
LDFLAGS = -lpthread

all: sample-threads sample-processes

sample-threads: sample.c
	$(CC) $(CFLAGS) $(LDFLAGS) sample.c -o Threads -DUSE_THREADS

sample-processes: sample.c
	$(CC) $(CFLAGS) $(LDFLAGS) sample.c -o Processes -DUSE_PROCS
  
clean:
	rm -f Threads Processes 

