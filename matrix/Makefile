all: tune_matrix run

OBJS = 
CC = g++
DEBUG = 
CFLAGS = -O3 -mavx2 -mfma -fopenmp
LFLAGS = -Wall $(DEBUG)

tune_matrix: matrix.cpp common.h
	$(CC) $(CFLAGS) $< -o $@
run: tune_matrix
	./$<


clean:
	$(RM) tune_matrix

