#Benchmark DML_MEM


## Compile

----------

To compile the benchmark, the first thing to do is to uncomment the corresponding line in the [root Cmake configuration file](src/CMakeLists.txt) : `add_subdirectory (benchmark_dml_mem)`.

Then, several options are available.

1. **MPI**

The benchmark can be launched on multiple cores by using MPI. 
To compile the benchmark with MPI the option should be used `-DDML_BUILD_MPI=TRUE`. 

2. **INSTRUCTION TYPE**

For Intel, the benchmark can be compiled using avx-512 instruction with the following option`-DDML_INST_TYPE=skylake-avx512 `.

3. **YAMB**

YAMB library can be used to monitor the benchmark performance by using the following option `-DDML_USE_YAMB=TRUE`. 
YAMB has to be enabled in the [root Cmake configuration file](src/CMakeLists.txt) by uncommenting the corresponding line `add_subdirectory (tool_yamb)`.

4. **DEBUG**

Debug traces can be enabled with the following cmake option `-DCMAKE_CXX_FLAGS="-DOP_DEBUG"`

6. **EXAMPLE**

`cmake .. -DDML_BUILD_MPI=TRUE -DDML_USE_YAMB=FALSE` `-DCMAKE_CXX_FLAGS="-DOP_DEBUG"`







## Execute

----------

Availables options can be printed with `-h` option.


## Example

---------

### Sequential

### MPI
```
#1GB 6 CORES STRIDE=8
srun -N 1 -n 6  numactl --physcpubind=0,1,4,5,8,9 ./bin/benchmark_dml_mem/dml_mpi --log 8 --matrixsize 1000 --stride 8 --measure 100
```
