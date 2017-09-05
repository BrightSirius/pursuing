#include "common.h"
#include "string.h"
#include "stdio.h"
#include "sys/mman.h"
#include "memset.h"

#define SIZE 1024*1024*1024

int main(){
    //void* p = malloc(SIZE);
    void *p = mmap(NULL, SIZE, PROT_WRITE|PROT_READ, MAP_PRIVATE|MAP_ANONYMOUS|MAP_POPULATE, -1, 0);
    struct timeval t1, t2;
    memset(p, 0, SIZE);
    gettime(&t1, NULL);
    //memset(p, 1, SIZE);
    //fast_memset256(p, SIZE, 1);
    fast_memset256_mp(p, SIZE, 1);
    gettime(&t2, NULL);
    
    printf("cost: %.6fs\n", TIMEs(t1, t2));
    printf("bindwidth: %.6f(GB/s)\n", 1.0 / TIMEs(t1, t2));
    munmap(p, SIZE);
    return 0;
}
