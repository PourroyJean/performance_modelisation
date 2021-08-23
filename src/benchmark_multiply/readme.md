#Benchmark Multiply

## Compile

-----------

To compile the benchmark, the first thing to do is to uncomment the corresponding line in the [root Cmake configuration file](src/CMakeLists.txt) : `add_subdirectory (benchmark_multiply)`.

Then, several options are available.

1. **OPEN MP**

The benchmark can be launched on multiple cores by using OpenMP. 
