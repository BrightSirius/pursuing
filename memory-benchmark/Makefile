all: malloc memory mmap bench run

OBJS = 
CC = gcc -O2
DEBUG = 
CFLAGS =  -fopenmp -mavx2
LFLAGS = -Wall $(DEBUG)

bench: bench.c
	$(CC) $(CFLAGS) $< -o $@
	#gcc -O0 -g $< -o $@
malloc: malloc.c memset.h
	$(CC) $(CFLAGS) $< -o $@
mmap: mmap.c memset.h
	$(CC) $(CFLAGS) $< -o $@
memory: memory.c memcpy.h
	$(CC) $(CFLAGS) $< -o $@
run: malloc memory mmap
	#./malloc
	#./mmap
	./bench
	./memory
	#numactl --cpunodebind=0 --membind=0 ./malloc
	#numactl --cpunodebind=0 --membind=0 ./mmap
	#numactl --cpunodebind=0 --membind=0 ./memory


clean:
	$(RM) malloc mmap memory bench

