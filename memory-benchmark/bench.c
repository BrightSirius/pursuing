#include "stdio.h"
#include "common.h"
#include "assert.h"
#include <string.h>
#include "memset.h"
#include "malloc.h"


#define SIZE (1<<30)

/*
void* memset_naive(char* p, int v, int size){
    int i;
    for(i=0; i < size; i++){
        p[i] = v;
    }
}

void* memset_128(char* p, int v, int size){
    fast_memset128(p, size, v);
}

void* memset_256(char* p, int v, int size){
    fast_memset256(p, size, v);
}

void* memset_256_mp(char* p, int v, int size){
    fast_memset256_mp(p, size, v);
}
*/

void bench(void* (*mmset)(char*, int, int), char* p, int v, int size){
    struct timeval t1, t2;
    gettime(&t1, NULL);
    mmset(p, v, size);
    gettime(&t2, NULL);
    int i;
    for(i = 0; i < SIZE; i++) assert(p[i] == v);
    printf("cost: %.6fs\n", TIMEs(t1, t2));
    printf("bindwidth: %.6f(GB/s)\n", 1.0 / TIMEs(t1, t2));

}

int main(){
    //char* p = malloc(SIZE);
    char* p = memalign(256, SIZE);
    memset(p, 1, SIZE);
    printf("---------naive-------------\n");
    bench(memset_naive, p, 1, SIZE);
    printf("---------naive memset----------\n");
    bench(memset, p, 1, SIZE);
    printf("---------memset stream 128----------\n");
    bench(memset_128, p, 1, SIZE);
    printf("---------memset stream 256----------\n");
    bench(memset_256, p, 1, SIZE);
    printf("---------memset 256 multithreading----------\n");
    bench(memset_256_mp, p, 1, SIZE);
    free(p);
    return 0;
}
