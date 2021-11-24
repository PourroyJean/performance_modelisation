# Performance optimisation repository
This repo should be used by any programmer interested by tunning or monitoring the performance of his applications.
The repo is composed of several tools :

1. **Benchmarks**
* [**Kernel Generator**](/src/benchmark_kernel_generator) : used to determine what are the different frequency used by your processor during different scenario (AVX1, AVX2, Turbo ON/OFF...)
* [**DML_MEM**](/src/benchmark_dml_mem) : a benchmark used to measure the performance of the memory subsystem by accessing memory with stride access
* [**Benchmark Multiply**](/src/benchmark_multiply) : Matrix multiplication benchmark implemented with several optimizations: blocking, OpenMP, GPU Offloading...


2. **Profiling tools**  
* [**YAMB**](/src/tool_yamb) : a tool used to monitor the memory bus traffic.
* [**Oprofile++**](/src/tool_oprofile) : a tool to find and extract hot spots from an application

3. **Other tools**
* [**Fast Plot**](/script/fastplot) :  small python utility for plotting data from a text file

**Each tool can be compiled separately and have its own settings. Each directory contains the corresponding *readme* file.**


# Install

Each tool/benchmark can be compiled separately. You can choose which one to compile by modifying the [Cmake configuration file](src/CMakeLists.txt) in **src/CMakeLists.txt**. For example, to only compile the Kernel Generator, you can use the following configuration:
```
add_subdirectory (benchmark_kernel_generator)
#add_subdirectory (benchmark_dml_mem)
#add_subdirectory (benchmark_multiply)
#add_subdirectory (tool_yamb)
#add_subdirectory(tool_oprofile)
```

Here is the simplest way to download and install our tools:
```bash
git clone git@github.com:PourroyJean/performance_modelisation.git performance_modelisation
cd performance_modelisation
mkdir build
cd build
vi ../src/CMakeLists.txt  #to choose which tool to compile 
cmake ..
make
```


## Requirement

* [CMAKE](https://cmake.org/) - An open-source, cross-platform family of tools designed to build, test and package software


## Other

We also provide :
* [KeKKak](src/benchmark_keccak): simple kernels is developped to have a fair comparison of microarchitectures on the Keccak algorithm 


## Authors

* **[Jean Pourroy](https://www.linkedin.com/in/pourroyjean/)** - PHD - HPE
* **[Patrick Demichel]()** - HPE

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
