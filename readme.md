# Intro to Performance optimisation
This package should be used by every ninja programmer interested by tunning or monitoring the performance of his applications.

## Tools
This package contains the following tools
*   kg: used to determine what are the different frequency used by your processor during different scenario (AVX1, AVX2, Turbo ON/OFF...)

## Benchmark
We also provide some benchmark:
* KeKKak: simple kernels is developped to have a fair comparison of microarchitectures on the Keccak algorithm 

# Install
Here is the simplest way to download and install our tools:
```bash
git clone git@github.com:PourroyJean/performance_modelisation.git performance_modelisation
cd performance_modelisation
mkdir build
cd build
cmake ..
make
```

# Quick example

Here is an example explaining how to use `kg`:
```
cd performance_modelisation/buid/bin/tool_kernel_generator/kg
./kg
```



## Requirement

* [CMAKE](https://cmake.org/) - An open-source, cross-platform family of tools designed to build, test and package software


## Authors

* **[Jean Pourroy](https://www.linkedin.com/in/pourroyjean/)** - PHD student at ENS Paris Saclay - HPE
* **[Fred]()** - Ninja
* **[Dml]()** - Ninja

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Hat tip to anyone who's code was used
* Inspiration
* etc
