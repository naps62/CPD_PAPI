CC=gcc
CFLAGS=-Wall -Wextra -O3
NBODY_FLAGS=-lpapi
GENFLAGS=

NBODY_EXE=n_body
GEN_EXE=gen
TEST_DIR=test_results

N=16384

default: gen nbody

nbody: main.c n_body.c errors.h 
	$(CC) $(CFLAGS) -o $(NBODY_EXE) main.c $(NBODY_FLAGS)

gen: gen.c
	$(CC) $(CFLAGS) -o $(GEN_EXE) gen.c $(GEN_FLAGS)

test: default
	./run_tests.sh

gen_run: gen
	./gen $(N) $(N).in

clean:
	rm -f gen n_body *.o
