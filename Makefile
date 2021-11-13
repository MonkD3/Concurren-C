CC=gcc
CFLAGS=-Wall -Werror # -g -march=native -std=gnu11 -fopt-info-vec
LIBS=-lpthread
SRCDIR=src
BUILDDIR=build

# Default target for make
all : compile_philo compile_readwrt compile_prodcons

compile_philo:
	$(CC) $(CFLAGS) -o $(BUILDDIR)/philosophes $(SRCDIR)/philosophes.c $(LIBS)

compile_readwrt:
	$(CC) $(CFLAGS) -o $(BUILDDIR)/readwrt $(SRCDIR)/readwrt.c $(LIBS)

compile_prodcons:
	$(CC) $(CFLAGS) -o $(BUILDDIR)/prodcons $(SRCDIR)/prodcons.c $(LIBS)


benchmark: all
	sh ./benchmark_src/benchmark.sh > ./benchmark_src/Data/measures.csv
	python ./benchmark_src/graphes.py 

clean:
	rm -f ./$(BUILDDIR)/*
	rm -f benchmark_src/Data/*
	rm -f benchmark_src/Graphs/*

.PHONY: clean