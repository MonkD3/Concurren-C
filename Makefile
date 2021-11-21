# Compilation vars
CC=gcc
CFLAGS=-Wall -Werror -g # -march=native -std=gnu11 -fopt-info-vec
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
OBJDIR=$(BUILDDIR)/objects
################################# Compilation targets ############################

# Default target for make, compile all files
all : philosophes readwrt prodcons spinlock

# Individual compilation
philosophes: $(SRCDIR)/philosophes.c $(SRCDIR)/cmnfunc.o $(SRCDIR)/primitives.o
	$(CC) $(CFLAGS) -Iheaders -o $(BUILDDIR)/$@ $^ $(LIBS)

readwrt: $(SRCDIR)/readwrt.c $(SRCDIR)/cmnfunc.o $(SRCDIR)/primitives.o
	$(CC) $(CFLAGS) -Iheaders -o $(BUILDDIR)/$@ $^ $(LIBS)

prodcons: $(SRCDIR)/prodcons.c $(SRCDIR)/cmnfunc.o $(SRCDIR)/primitives.o
	$(CC) $(CFLAGS) -Iheaders -o $(BUILDDIR)/$@ $^ $(LIBS)

spinlock: $(SRCDIR)/spinlock.c $(SRCDIR)/cmnfunc.o $(SRCDIR)/primitives.o 
	$(CC) $(CFLAGS) -Iheaders -o $(BUILDDIR)/$@ $^ $(LIBS)


# Compile .c file into object files.
%.o: %.c
	$(CC) -Iheaders $(CFLAGS) -o $@ -c $<

################################# Benchmarking targets ##########################

# Benchmark all files
benchmark: bench_philo bench_rdwrt bench_pc bench_spin

# Individual benchmarks
bench_philo: philosophes
	bash $(BENCHMARK) $(BUILDDIR)/$^ $(DATAOUT)/$^_posix.csv 1 "POSIX"
	bash $(BENCHMARK) $(BUILDDIR)/$^ $(DATAOUT)/$^_tatas.csv 1 "TATAS"
	$(PY) $(GRAPH) --datapath $(DATAOUT)/$^_posix.csv $(DATAOUT)/$^_tatas.csv \
				   --legend "mutex POSIX" "mutex TATAS" \
				   --graphpath $(GRAPHOUT)/$^ \
				   --title "Benchmark philosophes.c"

bench_rdwrt: readwrt
	bash $(BENCHMARK) $(BUILDDIR)/$^ $(DATAOUT)/$^_posix.csv 2 "POSIX"
	bash $(BENCHMARK) $(BUILDDIR)/$^ $(DATAOUT)/$^_tatas.csv 2 "TATAS"
	$(PY) $(GRAPH) --datapath $(DATAOUT)/$^_posix.csv $(DATAOUT)/$^_tatas.csv \
				   --legend "mutex POSIX" "mutex TATAS" \
				   --graphpath $(GRAPHOUT)/$^ \
				   --title "Benchmark readwrt.c"

bench_pc: prodcons
	bash $(BENCHMARK) $(BUILDDIR)/$^ $(DATAOUT)/$^_posix.csv 2 "POSIX"
	bash $(BENCHMARK) $(BUILDDIR)/$^ $(DATAOUT)/$^_tatas.csv 2 "TATAS"
	$(PY) $(GRAPH) --datapath $(DATAOUT)/$^_posix.csv $(DATAOUT)/$^_tatas.csv \
				   --legend "mutex POSIX" "mutex TATAS" \
				   --graphpath $(GRAPHOUT)/$^ \
				   --title "Benchmark prodcons.c"

bench_spin: spinlock 
	bash $(BENCHMARK) $(BUILDDIR)/$^ $(DATAOUT)/$^_tas.csv 1 "TAS"
	bash $(BENCHMARK) $(BUILDDIR)/$^ $(DATAOUT)/$^_tatas.csv 1 "TATAS"
	$(PY) $(GRAPH) --datapath $(DATAOUT)/$^_tas.csv $(DATAOUT)/$^_tatas.csv \
				   --legend "mutex POSIX" "mutex TATAS" \
				   --graphpath $(GRAPHOUT)/$^ \
				   --title "Benchmark spinlock.c"
################################# Cleaning targets ###############################

clean:
	rm -f ./$(BUILDDIR)/*
	rm -f ./$(SRCDIR)/*.o
	rm -f benchmark/Data/*
	rm -f benchmark/Graphs/*

.PHONY: clean