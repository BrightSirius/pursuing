#include "emmintrin.h"
#include "immintrin.h"
#include "assert.h"

void* memset_naive(char* bytes, int val, int size){
    int i;
    for(i=0; i < size; i++){
        bytes[i] = val;
    }
}

void* memset_256(char* bytes, int val, int size){
    int i;
    __m256i vals = _mm256_set1_epi8(val);
    for(i=0; i < size; i+=32){
        _mm256_store_si256((__m256i*)(bytes+i), vals);
    }
}

void* memset_mp(void* bytes,  int val, int size){
    int i, bid;
    int blk_cnt=(1<<10);
    assert(size % blk_cnt == 0);
    int blk_size = size / blk_cnt;;
#pragma omp parallel for private(i) num_threads(4)
    for(bid=0; bid < blk_cnt; bid++){
        memset(bytes+bid*blk_size, val, blk_size);
    }
}

void* fast_memset128(void* bytes, int val, int size){
    int step = 16;
    assert(size % step == 0);
    assert((long long)(bytes) % step == 0);
    __m128i vals = _mm_set1_epi8(val);
    int i;
    for(i = 0; i < size; i += 4*step){
        _mm_stream_si128((__m128i *)(bytes+i), vals);
        _mm_stream_si128((__m128i *)(bytes+i)+1, vals);
        _mm_stream_si128((__m128i *)(bytes+i)+2, vals);
        _mm_stream_si128((__m128i *)(bytes+i)+3, vals);
    }
}

void* fast_memset256(void* bytes, int val, int size){
    int step = 32;
    assert(size % step == 0);
    assert((long long)(bytes) % step == 0);
    __m256i vals = _mm256_set1_epi8(val);
    int i;
    for(i = 0; i < size; i += step * 4){
        _mm256_stream_si256((__m256i *)(bytes+i), vals);
        _mm256_stream_si256((__m256i *)(bytes+i)+1, vals);
        _mm256_stream_si256((__m256i *)(bytes+i)+2, vals);
        _mm256_stream_si256((__m256i *)(bytes+i)+3, vals);
        //_mm256_stream_si256((__m256i *)(bytes+i)+4, vals);
        //_mm256_stream_si256((__m256i *)(bytes+i)+5, vals);
        //_mm256_stream_si256((__m256i *)(bytes+i)+6, vals);
        //_mm256_stream_si256((__m256i *)(bytes+i)+7, vals);
    }
}

void fast_memset256_mp(void* bytes, int val, int size){
    int step = 32;
    assert(size % step == 0);
    assert((long long)(bytes) % step == 0);
    __m256i vals = _mm256_set1_epi8(val);
    int i, bid;
    int blk_cnt=(1<<10);
    assert(size % blk_cnt == 0);
    int blk_size = size / blk_cnt;;
#pragma omp parallel for private(i) num_threads(4)
    for(bid=0; bid < blk_cnt; bid++){
        for(i = 0; i < blk_size; i += step * 4){
            _mm256_stream_si256((__m256i *)(bytes+bid*blk_size+i), vals);
            _mm256_stream_si256((__m256i *)(bytes+bid*blk_size+i)+1, vals);
            _mm256_stream_si256((__m256i *)(bytes+bid*blk_size+i)+2, vals);
            _mm256_stream_si256((__m256i *)(bytes+bid*blk_size+i)+3, vals);
        }
    }
}
