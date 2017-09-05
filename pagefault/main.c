#include "common.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <memset.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/mman.h>

#define SIZE (1024*1024*1024)
#define STEP (1<<21)


#define MMAP
//#define MALLOC

//mmap syscall
#define LENGTH (1024*1024*1024)
#define PROTECTION (PROT_READ | PROT_WRITE)

#ifdef __ia64__
#define ADDR (void *)(0x8000000000000000UL)
#define FLAGS (MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB | MAP_FIXED)
#else
#define ELSE
#define ADDR (void *)(0x0UL)
#define FLAGS (MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB)
#endif

//#define TEST_BINDWIDTH


void bench(void* (*mmset)(void*, int, int), char* p){
    struct timeval t1, t2;
    struct timeval t3, t4;
    gettime(&t1, NULL);
    memset(p, 1, SIZE);
    gettime(&t2, NULL);
    memset(p, 2, SIZE);
    gettime(&t3, NULL);
    long long off1 = TIMEus(t1, t2);
    long long off2 = TIMEus(t2, t3);
    printf("page fault latency(us): %8lld. fault_set %8lld, hot_set %8lld\n", off1 - off2, off1, off2);

}

int main(){
    //printf("brk begin %p\n", sbrk(0));
#ifdef MALLOC
    //char* p = malloc(SIZE);
    char* p = memalign(2*1024*1024, SIZE);
#endif
#ifdef MMAP
#ifdef ELSE
    printf("ELSE\n");
#endif
    void* addr = mmap(ADDR, LENGTH, PROTECTION, FLAGS, -1, 0);
	if (addr == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
    char* p = addr;
#endif
    //printf("alloc %dKB\n", SIZE/1024);
    //printf("p %p\n", p);
    //printf("brk alloc %p\n", sbrk(0));
    //char* p = memalign(256, SIZE);

    //bench(memset, p);
    //bench(memset_naive, p);
    
    sleep(10);
    
#ifdef MALLOC
    free(p);
#elif defined(MMAP)
    munmap(addr, SIZE);
#endif

    
#ifdef TEST_BINDWIDTH
    char* q = malloc(SIZE);
    struct timeval t1, t2;
    //memset(q, 0, SIZE);

    gettime(&t1, NULL);
    memset(q, 1, SIZE);
    gettime(&t2, NULL);
    
    int i;
    //for(i = 0; i < SIZE; i++) assert(p[i] == 1);
    printf("cost: %.6fs\n", TIMEs(t1, t2));
    printf("bindwidth: %.6f(GB/s)\n", 1.0 / TIMEs(t1, t2));
    free(q);
#endif
    return 0;
}
