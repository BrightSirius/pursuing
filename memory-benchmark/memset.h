#include "_memset.h"


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

