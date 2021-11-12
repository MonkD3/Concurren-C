CC=gcc
CFLAGS=-Wall -Werror # -g -march=native -std=gnu11 -fopt-info-vec
LIBS=-lpthread
INCLUDES="" # Add files later 

# Default target for make
compile: main.c $(INCLUDES)
	$(CC) -Iheaders $(CFLAGS) -o $@ $^ $(LIBS)

benchmark: compile
	@echo "This is a benchmarking target"

%.o: %.c
	$(CC) -Iheaders $(CFLAGS) -o $@ -c $<

clean:
	rm -f src/*.o

.PHONY: clean