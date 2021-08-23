#Benchmark Multiply

## Compile

-----------

To compile the benchmark, the first thing to do is to uncomment the corresponding line in the [root Cmake configuration file](src/CMakeLists.txt) : `add_subdirectory (benchmark_multiply)`.

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


4. ** Example **
- Cray Sequential = ``cmake  ..  -DCMAKE_C_COMPILER=cc -DCMAKE_CXX_COMPILER=CC -DCMAKE_FC_COMPILER=ftn -DBM_OMP=FALSE -DBM_OMP_TARGET_GPU=FALSE``
- Cray OMP GPU = ``cmake  ..  -DCMAKE_C_COMPILER=cc -DCMAKE_CXX_COMPILER=CC -DCMAKE_FC_COMPILER=ftn -DBM_OMP=TRUE -DBM_OMP_TARGET_GPU=TRUE``
## Execute

----------

Available options can be printed with `-h` option.
- ``-L 100`` number of line for matrix A
- ``-C 100`` number of column for matrix A
- ``-V 1``   version to use
- ``-B 10`` size of block for *blocking* optimization


### Example

------------

