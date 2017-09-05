#include <stdio.h>
#include <string.h>
#include "common.h"

#include <immintrin.h>
#include <malloc.h>

#define N 4096
#define BLOCK 16
#define SIMD_WIDTH 8


//#define RIGHT
#define PERF

float A[N][N], ATran[N][N];
float B[N][N], BTran[N][N];
float C[N][N], SOL[N][N];

float toBW(int bytes, float sec){
    return bytes*1.0f/(1024*1024*1024) / sec;
}
float toGflops(int flop, float sec){
    return flop / 1e9f / sec;
}

void init(){
    srand(0);
    struct timeval t1, t2;
    gettimeofday(&t1, NULL);
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++)
            A[i][j] = random_float(), ATran[j][i] = A[i][j];
    }
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++)
            B[i][j] = random_float(), BTran[j][i] = B[i][j];
    }   

    gettimeofday(&t2, NULL);
    printf("init cost: %.6fs\n", TIMEs(t1, t2));
}

void print(float* mat){
    for(int i = 0; i < 10; i++) printf("%.6f ", mat[i]);
    printf("\n");
}

void clear_c(){
    memset(C, 0, sizeof(C));
    //printf("size of C: %lu\n", sizeof(C));
}

#define eps 1e-2
bool check_right(){
    int cnt = 5;
    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++)
        if(fabs(C[i][j]-SOL[i][j]) >= eps) {
            printf("expected %.6f while get %.6f at %d %d\n", SOL[i][j], C[i][j], i, j);
            cnt--;
            if(cnt) continue;
            return false;
        }
    return true;
}

void baseline(){
    struct timeval t1, t2;
    gettimeofday(&t1, NULL);
#pragma omp parallel for
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            float tmp = 0.0f;
            for(int k = 0; k < N; k++)
                tmp += A[i][k] * B[k][j];
            SOL[i][j] = tmp;
            //for(int k = 0; k < N; k++)
                //C[i][j] += A[i][k] * B[k][j];
        }
    }
    gettimeofday(&t2, NULL);
    float sec = TIMEs(t1, t2);
    printf("baseline cost: %.6fs.\n", sec);
}

void blocked(bool check){
    struct timeval t1, t2;
    gettimeofday(&t1, NULL);
    // BLOCK 16 fit in a cache line
    //int BLOCK = 16;
    for(int i = 0; i < N; i += BLOCK){
        for(int j = 0; j < N; j += BLOCK){
            for(int k = 0; k < N; k += BLOCK){
                // intra-block computation
                for(int bi=0; bi < BLOCK; bi++){
                    for(int bj=0; bj < BLOCK; bj++){
                        for(int bk=0; bk < BLOCK; bk++){
                            C[i+bi][j+bj] += A[i+bi][k+bk] * B[k+bk][j+bj];
                        }
                    }
                }
            }
        }
    }
    gettimeofday(&t2, NULL);
    float sec = TIMEs(t1, t2);
    printf("blocked cost: %.6fs.\n", sec);
    if(check){
        if(memcmp(C, SOL, sizeof(SOL)) == 0) printf("Blocked Passed check\n");
        else EXIT("WRONG IMPLEMENTION");
    }
}

//avx2 intra block
void blocked_avx2(bool check){
    clear_c();
    //avx2: 256bit/32(float) = 8-wide
    struct timeval t1, t2;
    gettimeofday(&t1, NULL);
    // BLOCK 16 fit in a cache line
    //int BLOCK = 16;

    __m256 cline, aline;
    for(int i = 0; i < N; i += BLOCK){
        for(int j = 0; j < N; j += BLOCK){
            for(int k = 0; k < N; k += BLOCK){
                // intra-block computation
                for(int bi=0; bi < BLOCK; bi++){
                    for(int bj=0; bj < BLOCK; bj+=SIMD_WIDTH){
                        cline = _mm256_load_ps(&C[i+bi][j+bj]);
                        for(int bk=0; bk < BLOCK; bk++){
                            aline = _mm256_set1_ps(A[i+bi][k+bk]);
                            cline = _mm256_fmadd_ps(aline, _mm256_load_ps(&B[k+bk][j+bj]), cline);
                            //C[i+bi][j+bj] += A[i+bi][k+bk] * B[k+bk][j+bj];
                        }
                        _mm256_store_ps(&C[i+bi][j+bj], cline);
                    }
                }
            }
        }
    }
    gettimeofday(&t2, NULL);
    float sec = TIMEs(t1, t2);
    printf("blocked avx2 cost: %.6fs.\n", sec);
    if(check){
        if(memcmp(C, SOL, sizeof(SOL)) == 0) printf("Blocked Passed check\n");
        else EXIT("WRONG IMPLEMENTION");
    }
}

//avx2 intra block, B is transposed, more friendly for cache reuse
void blocked_avx2_BTran(bool check){
    clear_c();
    //avx2: 256bit/32(float) = 8-wide
    struct timeval t1, t2;
    gettimeofday(&t1, NULL);
    // BLOCK 16 fit in a cache line
    //int BLOCK = 16;

#define SIMD_1

    for(int i = 0; i < N; i += BLOCK){
        for(int j = 0; j < N; j += BLOCK){
            for(int k = 0; k < N; k += BLOCK){
                // intra-block computation
                for(int bi=0; bi < BLOCK; bi++){
                    for(int bj=0; bj < BLOCK; bj++){
#ifdef SIMD_1
                        __m256 cline, aline,bline;
                        cline = _mm256_set1_ps(0.0f);
                        for(int bk=0; bk < BLOCK; bk+=SIMD_WIDTH){
                            aline = _mm256_load_ps(&A[i+bi][k+bk]);
                            bline = _mm256_load_ps(&BTran[j+bj][k+bk]);
                            cline = _mm256_fmadd_ps(aline, bline, cline);
                            //C[i+bi][j+bj] += A[i+bi][k+bk] * B[k+bk][j+bj];
                        }
                        float tmp=0.0f;
                        float* clines = (float*)&cline;
                        for(int in=0; in < SIMD_WIDTH; in++) tmp += clines[in];
                        C[i+bi][j+bj] += tmp;
#else 
                        float tmp=0.0f;
                        for(int bk=0; bk < BLOCK; bk++){
                            tmp += A[i+bi][k+bk] * BTran[j+bj][k+bk];
                        }
                        C[i+bi][j+bj] += tmp;
#endif
                    }
                }
            }
        }
    } 
    gettimeofday(&t2, NULL);
    float sec = TIMEs(t1, t2);
    printf("blocked avx2 BTran cost: %.6fs.\n", sec);
    if(check){
        if(check_right() == 0) printf("Blocked BTran Passed check\n");
        else EXIT("WRONG IMPLEMENTION");
    }
}

//avx2 intra block, A is transposed, C is tranposed.
void blocked_avx2_ATran(bool check){
    clear_c();
    //avx2: 256bit/32(float) = 8-wide
#define SIMD_WIDTH 8
    struct timeval t1, t2;
    gettimeofday(&t1, NULL);
    // BLOCK 16 fit in a cache line
    //int BLOCK = 16;

    for(int i = 0; i < N; i += BLOCK){
        for(int j = 0; j < N; j += BLOCK){
            for(int k = 0; k < N; k += BLOCK){
                // intra-block computation
                for(int bi=0; bi < BLOCK; bi+=SIMD_WIDTH){
                    for(int bj=0; bj < BLOCK; bj+=SIMD_WIDTH){
                        __m256 cline[SIMD_WIDTH];
                        for(int t=0; t < SIMD_WIDTH; t++)
                            cline[t] = _mm256_load_ps(&C[j+bj+t][i+bi]);
                        for(int bk=0; bk < BLOCK; bk++){
                            __m256 aline = _mm256_load_ps(&ATran[k+bk][i+bi]);
                            for(int t=0; t < SIMD_WIDTH; t++){
                                __m256 bline = _mm256_set1_ps(B[k+bk][j+bj+t]);
                                cline[t] = _mm256_fmadd_ps(aline, bline, cline[t]);
                            }
                        }
                        for(int t=0; t < SIMD_WIDTH; t++)
                            _mm256_store_ps(&C[j+bj+t][i+bi], cline[t]);
                    }
                }
            }
        }
    } 
    gettimeofday(&t2, NULL);
    float sec = TIMEs(t1, t2);
    printf("blocked avx2 ATran cost: %.6fs.\n", sec);
    if(check){
        for(int i = 0; i < N; i++ ){
            for(int j = i+1; j < N; j++) {
                float tmp = C[i][j];
                C[i][j] = C[j][i];
                C[j][i] = tmp;
            }
        }
        if(check_right()) printf("Blocked ATran Passed check\n");
        else EXIT("WRONG IMPLEMENTION");
    }
}


int main(){
    init();
    clear_c();
    //print(A[0]);
    //print(B[0]);
#ifdef RIGHT
    baseline();
    blocked(true);
    blocked_avx2(true);
    blocked_avx2_BTran(true);
    blocked_avx2_ATran(true);
#elif defined(PERF)
    blocked(false);
    blocked_avx2(false);
    blocked_avx2_BTran(false);
    blocked_avx2_ATran(false);
#endif
    return 0;
}
