
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
Below, the result of the exuction of `benchmark_multiply -V 6 -L 4000 -C 4000 -B 40` on AMD EPYC 7542 32-Core Processor
```
THREADS PROC_BIND OMP_PLACES VERSION BLOCK RES             TIME
1       false     threads    6       40    140100658790400 5:41.08
16      false     threads    6       40    140100658790400 0:22.76
16      false     cores      6       40    140100658790400 0:22.72
16      false     sockets    6       40    140100658790400 0:22.85
16      true      threads    6       40    140100658790400 0:22.94
16      true      cores      6       40    140100658790400 0:22.90
16      true      sockets    6       40    140100658790400 0:22.75
16      master    threads    6       0     ERROR           0:00.00
16      master    cores      6       0     ERROR           0:00.00
16      master    sockets    6       40    140100658790400 0:22.28
16      close     threads    6       40    140100658790400 0:43.25
16      close     cores      6       40    140100658790400 0:22.46
16      close     sockets    6       40    140100658790400 0:22.78
16      spread    threads    6       40    140100658790400 0:22.87
16      spread    cores      6       40    140100658790400 0:22.87
16      spread    sockets    6       40    140100658790400 0:22.60
32      false     threads    6       40    140100658790400 0:14.10
32      false     cores      6       40    140100658790400 0:13.93
32      false     sockets    6       40    140100658790400 0:14.06
32      true      threads    6       40    140100658790400 0:11.63
32      true      cores      6       40    140100658790400 0:11.48
32      true      sockets    6       40    140100658790400 0:11.63
32      master    threads    6       0     ERROR           0:00.00
32      master    cores      6       0     ERROR           0:00.00
32      master    sockets    6       40    140100658790400 0:13.80
32      close     threads    6       40    140100658790400 0:22.17
32      close     cores      6       40    140100658790400 0:11.29
32      close     sockets    6       40    140100658790400 0:11.61
32      spread    threads    6       40    140100658790400 0:11.59
32      spread    cores      6       40    140100658790400 0:11.53
32      spread    sockets    6       40    140100658790400 0:11.63
48      false     threads    6       40    140100658790400 0:12.88
48      false     cores      6       40    140100658790400 0:12.66
48      false     sockets    6       40    140100658790400 0:12.90
48      true      threads    6       40    140100658790400 0:07.76
48      true      cores      6       40    140100658790400 0:07.79
48      true      sockets    6       40    140100658790400 0:09.29
48      master    threads    6       0     ERROR           0:00.00
48      master    cores      6       0     ERROR           0:00.00
48      master    sockets    6       40    140100658790400 0:12.36
48      close     threads    6       40    140100658790400 0:14.79
48      close     cores      6       40    140100658790400 0:07.79
48      close     sockets    6       40    140100658790400 0:08.04
48      spread    threads    6       40    140100658790400 0:07.76
48      spread    cores      6       40    140100658790400 0:07.76
48      spread    sockets    6       40    140100658790400 0:09.05
64      false     threads    6       40    140100658790400 0:11.48
64      false     cores      6       40    140100658790400 0:11.51
64      false     sockets    6       40    140100658790400 0:11.43
64      true      threads    6       40    140100658790400 0:06.04
64      true      cores      6       40    140100658790400 0:06.04
64      true      sockets    6       40    140100658790400 0:08.57
64      master    threads    6       0     ERROR           0:00.00
64      master    cores      6       0     ERROR           0:00.00
64      master    sockets    6       40    140100658790400 0:11.55
64      close     threads    6       40    140100658790400 0:11.46
64      close     cores      6       40    140100658790400 0:05.99
64      close     sockets    6       40    140100658790400 0:08.60
64      spread    threads    6       40    140100658790400 0:05.99
64      spread    cores      6       40    140100658790400 0:06.02
64      spread    sockets    6       40    140100658790400 0:08.77
80      false     threads    6       40    140100658790400 0:08.65
80      false     cores      6       40    140100658790400 0:08.65
80      false     sockets    6       40    140100658790400 0:08.59
80      true      threads    6       40    140100658790400 0:09.01
80      true      cores      6       40    140100658790400 0:09.01
80      true      sockets    6       40    140100658790400 0:07.42
80      master    threads    6       0     ERROR           0:00.00
80      master    cores      6       0     ERROR           0:00.00
80      master    sockets    6       0     ERROR           0:00.00
80      close     threads    6       40    140100658790400 0:09.28
80      close     cores      6       40    140100658790400 0:09.25
80      close     sockets    6       40    140100658790400 0:07.38
80      spread    threads    6       40    140100658790400 0:09.03
80      spread    cores      6       40    140100658790400 0:09.00
80      spread    sockets    6       40    140100658790400 0:07.55
96      false     threads    6       40    140100658790400 0:07.40
96      false     cores      6       40    140100658790400 0:07.27
96      false     sockets    6       40    140100658790400 0:07.60
96      true      threads    6       40    140100658790400 0:07.73
96      true      cores      6       40    140100658790400 0:07.72
96      true      sockets    6       40    140100658790400 0:06.60
96      master    threads    6       0     ERROR           0:00.00
96      master    cores      6       0     ERROR           0:00.00
96      master    sockets    6       0     ERROR           0:00.00
96      close     threads    6       40    140100658790400 0:07.65
96      close     cores      6       40    140100658790400 0:07.72
96      close     sockets    6       40    140100658790400 0:06.76
96      spread    threads    6       40    140100658790400 0:07.67
96      spread    cores      6       40    140100658790400 0:07.66
96      spread    sockets    6       40    140100658790400 0:06.50
112     false     threads    6       40    140100658790400 0:06.52
112     false     cores      6       40    140100658790400 0:06.52
112     false     sockets    6       40    140100658790400 0:07.69
112     true      threads    6       40    140100658790400 0:06.64
112     true      cores      6       40    140100658790400 0:06.63
112     true      sockets    6       40    140100658790400 0:06.36
112     master    threads    6       0     ERROR           0:00.00
112     master    cores      6       0     ERROR           0:00.00
112     master    sockets    6       0     ERROR           0:00.00
112     close     threads    6       40    140100658790400 0:06.70
112     close     cores      6       40    140100658790400 0:06.71
112     close     sockets    6       40    140100658790400 0:06.38
112     spread    threads    6       40    140100658790400 0:06.66
112     spread    cores      6       40    140100658790400 0:06.64
112     spread    sockets    6       40    140100658790400 0:06.34
128     false     threads    6       40    140100658790400 0:06.02
128     false     cores      6       40    140100658790400 0:06.07
128     false     sockets    6       40    140100658790400 0:06.11
128     true      threads    6       40    140100658790400 0:05.94
128     true      cores      6       40    140100658790400 0:05.93
128     true      sockets    6       40    140100658790400 0:06.21
128     master    threads    6       0     ERROR           0:00.00
128     master    cores      6       0     ERROR           0:00.00
128     master    sockets    6       0     ERROR           0:00.00
128     close     threads    6       40    140100658790400 0:05.96
128     close     cores      6       40    140100658790400 0:05.97
128     close     sockets    6       40    140100658790400 0:06.10
128     spread    threads    6       40    140100658790400 0:05.93
128     spread    cores      6       40    140100658790400 0:05.91
128     spread    sockets    6       40    140100658790400 0:06.14 
```






------------

