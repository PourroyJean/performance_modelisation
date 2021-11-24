#Benchmark DML_MEM


The Kernel Generator tool makes it possible to measure certain characteristics of the FPU of a processor thanks to the execution of a kernel generated in assembler containing arithmetic instructions. 
The instructions used can be scalar or vector. The current version of the tool supports scalar, SSE, AVX2 and AVX512 instructions.

## Compile

----------

To compile the benchmark, the only thing to do is to uncomment the corresponding line in the [root Cmake configuration file](src/CMakeLists.txt) : `add_subdirectory (benchmark_kernel_generator)` then use the following command:

`cd build ; cmake .. `




## Execute

----------

Available options can be printed with `-h` option.

```text 

  [OPTION NAME]                 [PARAMETERS]        [DEFINITION]
  
  -W,  --width                  (64|128|256|512)    Define the width of the vector instructions used. On an Intel architecture, 
                                                    these instructions correspond to the following ISA: MMX (64), SSE (128), AVX (256) and AVX-512 (512). 
                                                    For the moment, vector instructions of different sizes cannot be mixed in the same kernel.
  
  -w,  --width_cust             [1,2,3,4]+          Mix different operation width : 1(scalar), 2 (SSE/128), 3 (AVX/256), 4(AVX/512)
  
  -O,  --operations             [a,m,f]+            List the operations to be performed by the kernel using a string composed of the letters a, m and f 
                                                    corresponding to the following operations: addition (a), multiplication (m)
                                                    or merged multiplication and addition (FMA) (f)
  
  -C,  --compiler               g++ --std=c++11     Use your own compiler to compile the kernel
  
  -B,  --binding                N                   Bind the generated benchmark to a specific core N. Since the benchmark is not parallelized, 
                                                    it is necessary to run several versions in parallel to test the performance of a processor when several cores are used. 
                                                    The different processes created can then be binded to the different cores of the processor using an external script.
  
  -D,  --dependency             N                   Generate an instruction whose operand is the result produced by the previous instruction. 
                                                    A number N can be given to generate multiple dependency strings. This option is particularly useful 
                                                    for measuring instruction latency and execution buffer performance in disorder.
  
  -P,  --precision              (single|double)     Define the precision used to perform the calculations: single or double. 
                                                    For the moment, instructions with different precisions cannot be mixed in the same kernel.
  
  -L,  --loopsize               N                   Improve the accuracy of results and eliminate potential measurement noise by performing N measurements.
  
  -S,  --samples                N                   Number of execution of the kernel (int NB_lOOP)
  
  -U,  --unrolling              N                   Apply the loop unwinding optimization N times. 
                                                    This optimization allows the loop body to be unwound multiple times within the loop to reduce 
                                                    the impact of processing loop control instructions (incrementing and comparing) on benchmark performance.
  
  -F,  --frequency              (true|false)        Generate and execute a function at the beginning of the benchmark that measures the processor frequency. 
                                                    The calculation of the results are impacted by the use of the turbo mode. 
                                                    By knowing the value of the turbo frequency, these results can be adjusted.
  
  -G,  --graphic                                    Generate histograms (requires python) 
  
  -A,  --debug                  
  
  -v,  --verbose                
  
  -h,  --help                   
```

## Example


