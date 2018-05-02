all: fault

OUTs = fault ingens

OBJS = 
CC = gcc
DEBUG = 
CFLAGS =  
LFLAGS = -Wall $(DEBUG)

fault: main.c
	#$(CC) $(CFLAGS) $< -o $@
	gcc -mavx -O0 -g $< -I /home/sirius/labs-proj/pursuing/memory-benchmark/ -o $@
ingens: ingens-fault.c
	gcc $< -o $@ -lpthread
	

run: $(OUTs)
	./fault
	#numactl --cpunodebind=0 --membind=0 ./malloc

clean:
	$(RM) $(OUTs)
	$(RM) perf*

