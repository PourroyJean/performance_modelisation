
# Benchmark Multiply

Matrix multiplication benchmark implemented with several optimizations: blocking, OpenMP, GPU Offloading...
Both matrices have the same size.


## Compile

-----------

To compile the benchmark, the first thing to do is to uncomment the corresponding line in the [root Cmake configuration file](/src/CMakeLists.txt) : `add_subdirectory (benchmark_multiply)`.

Then, several options are available.

1. **OpenMP**

The benchmark can be launched on multiple cores by using OpenMP.
To compile the benchmark with OPENMP use the following option `-DBM_OMP=TRUE`. 

2. **OpenMP GPU offloading**

The benchmark can be launched on a GPU with OpenMP (version > 4.5)
To offload the benchmark on GPU, use the following option `-DBM_OMP_TARGET_GPU=TRUE`

3. **Manual Compilation**

The benchmark can be compiled without using Cmake
` /opt/rocm/llvm/bin/clang++ -DBM_OMP -DBM_OMP_TARGET_GPU -fopenmp ../src/benchmark_multiply/benchmark_multiply.cpp ../src/benchmark_multiply/multiply_version.cpp -fopenmp -fopenmp-targets=amdgcn-amd-amdhsa -Xopenmp-target=amdgcn-amd-amdhsa -march=gfx908  -o mult_manocompiled`


4. **Example**
- Cray Sequential = ``cmake  ..  -DCMAKE_C_COMPILER=cc -DCMAKE_CXX_COMPILER=CC -DCMAKE_FC_COMPILER=ftn -DBM_OMP=FALSE -DBM_OMP_TARGET_GPU=FALSE``
- Cray OMP GPU = ``cmake  ..  -DCMAKE_C_COMPILER=cc -DCMAKE_CXX_COMPILER=CC -DCMAKE_FC_COMPILER=ftn -DBM_OMP=TRUE -DBM_OMP_TARGET_GPU=TRUE``
---------------------------

## Execute

----------

Available options can be printed with `-h` option.
- ``-L 100`` number of line for matrix A
- ``-C 100`` number of column for matrix A
- ``-V 1``   version to use
- ``-B 20`` size of block for *blocking* optimization

Typical execution
`` export OMP_PROC_BIND=close ; export OMP_PLACES=cores ; export OMP_NUM_THREADS=128 ; time ./bin/benchmark_multiply/benchmark_multiply -V 6 -L 4000 -C 4000
``

---------------------------
## Script

---------------------------
Several scripts can be found in the [script folder](/src/benchmark_multiply/script)

1. **[bm_execute.sh](/src/benchmark_multiply/script/bm_execute.sh)**

Script to facilitate the execution and your different tests. Nothing special here. 


2. **[bm_omp_benchmark.sh](/src/benchmark_multiply/script/bm_omp_benchmark.sh)**

This script can be used to test different configuration of the OpenMP binding variables (**OMP_NUM_THREADS**, **OMP_PROC_BIND**, **OMP_PLACES**) and different size of block.
Then, for different number of threads, binding and place strategies the script print the performance.
Below, the result of the exuction of `benchmark_multiply -V 6 -L 4000 -C 4000 -B 40` on AMD EPYC 7542 32-Core Processor (see full results in [results folder](/results/benchmark_multiply/benchmark_multiply%20-V%206%20-L%204000%20-C%204000%20-B%2040.txt))

```
THREADS PROC_BIND OMP_PLACES VERSION BLOCK RES            TIME      
1       true      cores      6       40    140100658790400 585.44    
2       true      cores      6       40    140100658790400 182.41    
3       true      cores      6       40    140100658790400 116.60    
.......     
122     true      cores      6       40    140100658790400 6.19      
123     true      cores      6       40    140100658790400 6.23      
124     true      cores      6       40    140100658790400 6.16   
```


Strong scaling on dual socket server for 1 to 128 threads with `OMP_PROC_BIND=true` and `OMP_PROC_PLACES=cores`. 
![Alt text](/results/benchmark_multiply/V6_1-128_threads.png?raw=true "Strong scaling")






------------

