
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

1. **[bm_execute.sh](/src/benchmark_multiply/script/bm_omp_execute.sh)**

Script to facilitate the execution and your different tests. Nothing special here. 


2. **[bm_omp_cpu_bind.sh](/src/benchmark_multiply/script/bm_omp_cpu_bind.sh)**

This script can be used to test different configuration of the OpenMP binding variables : **OMP_NUM_THREADS**, **OMP_PROC_BIND** and **OMP_PLACES**.
Then, for different number of threads, binding and place strategies the script print the performance.
Below, the result of the exuction of `benchmark_multiply -V 6 -L 4000 -C 4000 -B 40` on AMD EPYC 7542 32-Core Processor
```
THREADS  PROC_BIND OMP_PLACES VERSION TIME       
32       false     threads    6       0:11.88    
32       false     cores      6       0:11.80    
32       false     sockets    6       0:11.70    
32       true      threads    6       0:10.11    
32       true      cores      6       0:10.10    
32       true      sockets    6       0:10.17    
32       master    threads    6       ERROR      
32       master    cores      6       ERROR      
32       master    sockets    6       0:12.60    
32       close     threads    6       0:19.25    
32       close     cores      6       0:10.14    
32       close     sockets    6       0:10.09    
32       spread    threads    6       0:10.07    
32       spread    cores      6       0:10.09    
32       spread    sockets    6       0:10.28    
64       false     threads    6       0:10.17    
64       false     cores      6       0:10.19    
64       false     sockets    6       0:10.13    
64       true      threads    6       0:05.75    
64       true      cores      6       0:05.70    
64       true      sockets    6       0:08.19    
64       master    threads    6       ERROR      
64       master    cores      6       ERROR      
64       master    sockets    6       0:10.45    
64       close     threads    6       0:10.14    
64       close     cores      6       0:05.67    
64       close     sockets    6       0:08.08    
64       spread    threads    6       0:05.69    
64       spread    cores      6       0:05.65    
64       spread    sockets    6       0:07.99    
128      false     threads    6       0:06.33    
128      false     cores      6       0:05.86    
128      false     sockets    6       0:05.79    
128      true      threads    6       0:05.42    
128      true      cores      6       0:05.44    
128      true      sockets    6       0:05.96    
128      master    threads    6       ERROR      
128      master    cores      6       ERROR      
128      master    sockets    6       ERROR      
128      close     threads    6       0:05.42    
128      close     cores      6       0:05.48    
128      close     sockets    6       0:06.23    
128      spread    threads    6       0:05.43    
128      spread    cores      6       0:05.45    
128      spread    sockets    6       0:06.10 
```






------------

