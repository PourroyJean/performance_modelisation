# Intro to Performance optimisation
This package should be used by any programmer interested by tunning or monitoring the performance of his applications.

## Tools
This package contains the following tools
*   Kernel Generator: used to determine what are the different frequency used by your processor during different scenario (AVX1, AVX2, Turbo ON/OFF...)
*   DML_MEM: a benchmark used to measure the performance of the memory subsystem by accessing memory with stride access
*   YAMB: a tool used to monitor the memory bus traffic.
*   Oprofile++: a tool to find and extract hot spots from an application

# Install

Each tool/benchmark can be compiled separately. You can choose which one to compile by modifying the [Cmake configuration file](src/CMakeLists.txt) in **src/CMakeLists.txt**:
```
add_subdirectory (benchmark_kernel_generator)
#add_subdirectory(tool_oprofile)
#add_subdirectory (tool_yamb)
#add_subdirectory (benchmark_dml_mem)
```

Here is the simplest way to download and install our tools:
```bash
git clone git@github.com:PourroyJean/performance_modelisation.git performance_modelisation
cd performance_modelisation
mkdir build
cd build
cmake ..
make
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
