#include "common.h"
#include "string.h"
#include "stdio.h"
#include "memset.h"
#include "malloc.h"

#define SIZE 1024*1024*1024

int main(){
    //char* p = malloc(SIZE);
    char* p = memalign(256, SIZE);
    struct timeval t1, t2;
    memset(p, 0, SIZE);
    gettime(&t1, NULL);
    //memset(p, 1, SIZE);
    //memset_mp(p, SIZE, 1);
    //fast_memset128(p, SIZE, 1);
    //fast_memset256(p, SIZE, 1);
    fast_memset256_mp(p, SIZE, 1);
    gettime(&t2, NULL);
    
	int i;
    for(i = 0; i < SIZE; i++) assert(p[i] == 1);
    printf("cost: %.6fs\n", TIMEs(t1, t2));
    printf("bindwidth: %.6f(GB/s)\n", 1.0 / TIMEs(t1, t2));
    free(p);
    return 0;
}
