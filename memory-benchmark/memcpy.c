#include<sys/time.h>
#include<string.h>
#include<stdio.h>
#include<omp.h>
#include "memcpy.h"
#include "malloc.h"

#define DATA_SIZE (1<<30)
#define SEG_SIZE (1<<24)

char *src,*dst;

int main(){
    struct timeval begin,end,result;
    int i,j;
    double time;
    int times=1;
    //src = calloc(DATA_SIZE,1);
    //dst = calloc(DATA_SIZE,1);
    src = memalign(1024, DATA_SIZE);
    dst = memalign(1024, DATA_SIZE);
    //printf("src %p\n", src);
    //printf("dst %p\n", dst);
    memset(src,1,DATA_SIZE);
    memset(dst,0,DATA_SIZE);
    //printf("begin copy\n");
    //#pragma omp parallel //	#pragma omp master
    //		{
    //		printf("thread count %d\n", omp_get_num_threads());
    //		}
    gettimeofday(&begin,NULL);
    for(i=0;i<times;i++){
        int blocks=DATA_SIZE / SEG_SIZE;
//#pragma omp parallel for
        for(j=0;j<blocks;j++){
            memcpy256(dst + DATA_SIZE/blocks * j,src + DATA_SIZE/blocks * j,DATA_SIZE/blocks);
        //fast_memcpy256(dst + DATA_SIZE/blocks * j,src + DATA_SIZE/blocks * j,DATA_SIZE/blocks);
        //memcpy(dst + DATA_SIZE/blocks * j,src + DATA_SIZE/blocks * j,DATA_SIZE/blocks);
        }

    }
    gettimeofday(&end,NULL);
    timersub(&end,&begin,&result);
    printf("Time for copying %dGB data:%ld.%03lds\n",times,result.tv_sec,result.tv_usec/1000);
    time = result.tv_sec + result.tv_usec * 1e-6;
    printf("Memcpy bandwidth:%.2fGB/s\n",1.0 / time * times);
    for(i=0; i<DATA_SIZE; i++) 
        if(dst[i] != 1){
            printf("expect %d at %d while get %d\n", 1, i, dst[i]);
            break;//assert(dst[i]==1);
        }
    free(src);
    free(dst);
    return 0;
}
