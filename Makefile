# Compilation vars
CC=gcc
CFLAGS=-Wall -Werror # -g -march=native -std=gnu11 -fopt-info-vec
LIBS=-lpthread

# Benchmarking vars
PY=python3
BENCHMARK=./benchmark/perf_test.sh 
GRAPH=./benchmark/graphes.py
GRAPHOUT=./benchmark/Graphs
DATAOUT=./benchmark/Data

# Useful directories
SRCDIR=src
BUILDDIR=build

################################# Compilation targets ############################

# Default target for make, compile all files
all : philosophes readwrt prodcons

# Individual compilation
philosophes: $(SRCDIR)/philosophes.c $(SRCDIR)/cmnfunc.o
	$(CC) $(CFLAGS) -Iheaders -o $(BUILDDIR)/$@ $^ $(LIBS)

readwrt: $(SRCDIR)/readwrt.c $(SRCDIR)/cmnfunc.o
	$(CC) $(CFLAGS) -Iheaders -o $(BUILDDIR)/$@ $^ $(LIBS)

prodcons: $(SRCDIR)/prodcons.c $(SRCDIR)/cmnfunc.o
	$(CC) $(CFLAGS) -Iheaders -o $(BUILDDIR)/$@ $^ $(LIBS)

# Compile .c file into object files.
%.o: %.c
	$(CC) -Iheaders $(CFLAGS) -o $@ -c $<

################################# Benchmarking targets ##########################

# Benchmark all files
benchmark: bench_philo bench_rdwrt bench_pc

# Individual benchmarks
bench_philo: philosophes
	bash $(BENCHMARK) $(BUILDDIR)/$^ $(DATAOUT)/$^.csv 1
	$(PY) $(GRAPH) --datapath $(DATAOUT)/$^.csv --graphpath $(GRAPHOUT)/$^ --title "Benchmark philosophes.c"

bench_rdwrt: readwrt
	bash $(BENCHMARK) $(BUILDDIR)/$^ $(DATAOUT)/$^.csv 2
	$(PY) $(GRAPH) --datapath $(DATAOUT)/$^.csv --graphpath $(GRAPHOUT)/$^ --title "Benchmark readwrt.c"

bench_pc: prodcons
	bash $(BENCHMARK) $(BUILDDIR)/$^ $(DATAOUT)/$^.csv 2
	$(PY) $(GRAPH) --datapath $(DATAOUT)/$^.csv --graphpath $(GRAPHOUT)/$^ --title "Benchmark prodcons.c"


################################# Cleaning targets ###############################

clean:
	rm -f ./$(BUILDDIR)/*
	rm -f ./$(SRCDIR)/*.o
	rm -f benchmark/Data/*
	rm -f benchmark/Graphs/*

.PHONY: clean