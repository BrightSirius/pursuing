# matrix
## Why
pursuing how to optimize the performance of dense matrix multiple operation(single thread).

## What
5 methods are pursued.
1. naive version as baseline, $O(N^3)$ memory access. 1X
2. block based, $O(N^3/BLOCK)$ memory access. 6X
3. block, avx2. 10X
4. block, avx2. Matrix B is transposed. 5X
5. block, avx2. Matrix A and C are transposed. 6X

## Result
- **10X** speedup is achieved by method 3.
- Block size 16 * 16 is the best, which confilts with the fact that arithmetic intensity equals to BLOCK and bigger BLOCK will lead to better performance.

## Analysis
Single thread throughput: 4GHz(frequency) * 4(superscalar, 4 SIMD ALUs) * 8(AVX2, 8-wide) = 128Gflops, about 512GB/s.

Memroy throughput is about 20GB/s.


