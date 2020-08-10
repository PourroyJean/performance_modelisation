# Intro to Performance optimisation
This package should be used by any programmer interested by tunning or monitoring the performance of his applications.
This package contains the following tools:
*   Kernel Generator: used to determine what are the different frequency used by your processor during different scenario (AVX1, AVX2, Turbo ON/OFF...)
*   DML_MEM: a benchmark used to measure the performance of the memory subsystem by accessing memory with stride access
*   YAMB: a tool used to monitor the memory bus traffic.
*   Oprofile++: a tool to find and extract hot spots from an application

# Install

Each tool/benchmark can be compiled separately. You can choose which one to compile by modifying the [Cmake configuration file](src/CMakeLists.txt) in **src/CMakeLists.txt**. For example, to only compile the Kernel Generator, you can use the following configuration:
```
add_subdirectory (benchmark_kernel_generator)
#add_subdirectory(tool_oprofile)
#add_subdirectory (tool_yamb)
#add_subdirectory (benchmark_dml_mem)
```
*Note:* YAMB requires to be compiled with YAMB as it uses its *annotation code* library.

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

## Configure each tool

Each tool can be compiled separately and have its own settings.

### DML_MEM benchmark
The DML benchmark can be compiled with MPI or not. To do this, the following flag must be set in the cmake tool configuration:
```
cmake -DDML_BUILD_MPI=ON ..
```
To analyze the different steps of the benchmark (change of stride size, data set size) it can be interesting to monitor the memory bus traffic with YAMB and to annotate the graph with the library provided for. To do this, the following flag must be set in the cmake tool configuration:
```
cmake -DDML_USE_YAMB=ON ..
```

## Requirement

* [CMAKE](https://cmake.org/) - An open-source, cross-platform family of tools designed to build, test and package software


## Other

We also provide :
* KeKKak: simple kernels is developped to have a fair comparison of microarchitectures on the Keccak algorithm 



## Authors

* **[Jean Pourroy](https://www.linkedin.com/in/pourroyjean/)** - PHD student at ENS Paris Saclay - HPE
* **[Patrick Demichel]()** - HPE

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
