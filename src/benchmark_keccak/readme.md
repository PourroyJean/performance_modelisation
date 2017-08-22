
This suite of simple kernels is developped to have a fair comparison of microarchitectures on the Keccak algorithm
Fair means we have tuned algorithms using all new parallelisms or features available
This is intended to be used as a proxy to evaluate how those architectures would perform on this class of "crypto" codes or crypto moneys
There are many more codes that can be developped in this suite, we hope people will extend this suite with more codes, or tuned variants or measures on a wide range of hardwares

A large proportion of the crypto codes have a similar behavior
1: use only boolean or integer operations ; use NO floating point operations
2: present a massive parallelism ; generally many parallelisms if not all can be exploited
3: consist in long chains of operations presenting few parallelisms inside a chain ; but we have a huge number of chains to compute; this make the instruction parallelism more difficult to use
4: do not stress the memory subsystem ; do not require large memory
5: scale with the cores if no thermal limitation, scale with the clock and most compute parallelism mecanisms relativelly easily; but not always; this is what this suite need to discover
6: very sensitive to BUGS and hardware problems ; any temporal problem will generate a totally different checksum ; then the benchmark is very robust

In this case the main challenge is this long chain of dependencies between the integer operations 

The benchmarks should have the minimal number of dependencies : SHOULD depend only on gcc ; but we can give the receit to build with others compilers or exotic platforms
We do not use OpenMP and MPI to avoid proliferation of variants and dependencies 
The methodology choosen to evaluate in charge "using all compute elements" is to run multiple processes with similar parameters
It is expected to show the performance when using 1 compute element "ex: 1 core" , and all compute elements "all cores" ; you simply need to multiply the performance of 1 core by the number of cores to get the aggregate performance of the socket ; multiply it by the number of sockets to get the performance of the node
All iterations have a timing ; it is expected the timings are ultra stable; but thermal constraints can force the processors to throttle the clock; then affecting temporally the performance
The expectation is that the results SHOULD be easy to reproduce and validate
But we can also deliver some variants coded and potentially tuned for GPU, FPGA, and all sorts of accelerators that could be good candidates

The Keccak suite consists in multiple codes
1: reference code : bench_keccakf_ref.cpp      : using the original algorithm ; not tuned but generating some checksums to assert that the computations produce the correct results
2: tuned version  : bench_keccakf_tuned.cpp    : a tuned version 
3: AVX256         : bench_keccakf_quad.cpp     : a vectorized version for AVX2
3: AVX512         : bench_keccakf_octo.cpp     : a vectorized version for AVX512
4: CUDA           : bench_keccakf_tuned.cu     : a cuda version


All thoses versions can be tuned more ; we are looking for ultimate performance assuming the checksums are correct
You can transform the benchmark as you wish to get more parallelism if you produce the right checksums 
You can increase the value of the 2nd parameter "number of chains to compute" if this is usefull to get a better performance; but you need to prove your checksum is correct
The 3rd parameter ; the iteration loop SHOULD not be changed ; this would change the checksum ; but is offered as a mecanism to run smaller problems 
The order of the computationis of the chains and the intermediate checksums is of NO importance ; you can compute  in any order but need to compute the chains n any order ; is you compute all the chains the final checksum will be correct

We need this final checksum and the Mks "MegaKeccak per second" metric 
You should report the metric and using the full component capabilities ; use all cores or all units 
We do not deliver the mecanisms for exercising the parallelisms of the components ; for exemple you can run 20 similar jobs ; then sum the individual Mks and report the sum 
 

========================================================================================================================================
processor : model name      : Intel(R) Xeon(R) Silver 4110 CPU @ 2.10GHz 2 SOCKETS*8cores
========================================================================================================================================
 icc -O bench_keccakf_ref.cpp -o bench_keccakf_ref_icc
 bench_keccakf_ref_icc
_S_          0 ac6918402e4d3900 ac6918402e4d3900     1.427213
_S_          1 cf0269634671e842 636b7123683cd142     1.431792
_S_          2 4c2ce1c0a38b54f3 2f4790e3cbb785b1     1.426703
----------------------------------------------------------------------------------------------------------------------------------------
 icc -O3 -x=core-avx2 bench_keccakf_ref.cpp -o bench_keccakf_ref_icc
 publicable]$ ./bench_keccakf_ref_icc
_S_          0 ac6918402e4d3900 ac6918402e4d3900     1.338454
_S_          1 cf0269634671e842 636b7123683cd142     1.338262
_S_          2 4c2ce1c0a38b54f3 2f4790e3cbb785b1     1.339961
----------------------------------------------------------------------------------------------------------------------------------------
 icc -O3 -x=core-avx512 bench_keccakf_ref.cpp -o bench_keccakf_ref_icc
 bench_keccakf_ref_icc
_S_          0 ac6918402e4d3900 ac6918402e4d3900     3.202901
_S_          1 cf0269634671e842 636b7123683cd142     3.203751
_S_          2 4c2ce1c0a38b54f3 2f4790e3cbb785b1     3.203557
----------------------------------------------------------------------------------------------------------------------------------------
 gcc -O3 bench_keccakf_ref.cpp -o bench_keccakf_ref_cpp
 publicable]$ bench_keccakf_ref_cpp
_S_          0 ac6918402e4d3900 ac6918402e4d3900     1.363096
_S_          1 cf0269634671e842 636b7123683cd142     1.361470
_S_          2 4c2ce1c0a38b54f3 2f4790e3cbb785b1     1.361990
_S_          3 c23846f289232c2d ed7fd6114294a99c     1.364584
----------------------------------------------------------------------------------------------------------------------------------------
16 jobs in parallel:
more jmany
for i in {0..15};do
        numactl  --physcpubind=$i $*&
done
----------------------------------------------------------------------------------------------------------------------------------------
 jmany bench_keccakf_tuned_icc
_S_          0 ac6918402e4d3900 ac6918402e4d3900     1.672716
_S_          0 ac6918402e4d3900 ac6918402e4d3900     1.672653
_S_          0 ac6918402e4d3900 ac6918402e4d3900     1.672557
_S_          0 ac6918402e4d3900 ac6918402e4d3900     1.672340
_S_          0 ac6918402e4d3900 ac6918402e4d3900     1.674083
_S_          0 ac6918402e4d3900 ac6918402e4d3900     1.673134
_S_          0 ac6918402e4d3900 ac6918402e4d3900     1.672486
_S_          0 ac6918402e4d3900 ac6918402e4d3900     1.672192
_S_          0 ac6918402e4d3900 ac6918402e4d3900     1.675084
_S_          0 ac6918402e4d3900 ac6918402e4d3900     1.672348
_S_          0 ac6918402e4d3900 ac6918402e4d3900     1.672655
_S_          0 ac6918402e4d3900 ac6918402e4d3900     1.672441
_S_          0 ac6918402e4d3900 ac6918402e4d3900     1.672458
_S_          0 ac6918402e4d3900 ac6918402e4d3900     1.672411
_S_          0 ac6918402e4d3900 ac6918402e4d3900     1.677014
_S_          0 ac6918402e4d3900 ac6918402e4d3900     1.675698
_S_          1 cf0269634671e842 636b7123683cd142     1.672381
_S_          1 cf0269634671e842 636b7123683cd142     1.672419
_S_          1 cf0269634671e842 636b7123683cd142     1.672347
_S_          1 cf0269634671e842 636b7123683cd142     1.671976
_S_          1 cf0269634671e842 636b7123683cd142     1.672085
_S_          1 cf0269634671e842 636b7123683cd142     1.672675
_S_          1 cf0269634671e842 636b7123683cd142     1.671958
_S_          1 cf0269634671e842 636b7123683cd142     1.672019
_S_          1 cf0269634671e842 636b7123683cd142     1.672050
_S_          1 cf0269634671e842 636b7123683cd142     1.672139
_S_          1 cf0269634671e842 636b7123683cd142     1.672345
_S_          1 cf0269634671e842 636b7123683cd142     1.672600
_S_          1 cf0269634671e842 636b7123683cd142     1.673611
_S_          1 cf0269634671e842 636b7123683cd142     1.672085
_S_          1 cf0269634671e842 636b7123683cd142     1.672080
_S_          1 cf0269634671e842 636b7123683cd142     1.681232
----------------------------------------------------------------------------------------------------------------------------------------
CONCLUSION:
1: best option for compiler is icc -O3 -x=core-avx2
2: -x=core-avx512 is not efficient
3: running 16 jobs degrades 25%
========================================================================================================================================
 icc -O3 -x=core-avx2 bench_keccakf_tuned.cpp -o bench_keccakf_tuned_icc
 bench_keccakf_tuned_icc
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.493630
_S_          1 cf0269634671e842 636b7123683cd142     0.493234
_S_          2 4c2ce1c0a38b54f3 2f4790e3cbb785b1     0.493378
_S_          3 c23846f289232c2d ed7fd6114294a99c     0.493224
_S_          4 82a600ea7029e730 6fd9d6fb32bd4eac     0.493693
----------------------------------------------------------------------------------------------------------------------------------------
 gcc -O3 bench_keccakf_tuned.cpp -o bench_keccakf_tuned_gcc
 publicable]$ bench_keccakf_tuned_gcc
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.856292
_S_          1 cf0269634671e842 636b7123683cd142     0.851146
_S_          2 4c2ce1c0a38b54f3 2f4790e3cbb785b1     0.849197
----------------------------------------------------------------------------------------------------------------------------------------
 jmany bench_keccakf_tuned_icc
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.618157
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.617979
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.618132
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.618079
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.618396
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.618334
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.617940
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.618049
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.617998
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.618832
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.618077
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.618042
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.617818
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.617995
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.617813
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.618034
_S_          1 cf0269634671e842 636b7123683cd142     0.617634
_S_          1 cf0269634671e842 636b7123683cd142     0.617764
_S_          1 cf0269634671e842 636b7123683cd142     0.617679
_S_          1 cf0269634671e842 636b7123683cd142     0.617663
_S_          1 cf0269634671e842 636b7123683cd142     0.617685
_S_          1 cf0269634671e842 636b7123683cd142     0.617755
_S_          1 cf0269634671e842 636b7123683cd142     0.617739
_S_          1 cf0269634671e842 636b7123683cd142     0.617626
_S_          1 cf0269634671e842 636b7123683cd142     0.617777
_S_          1 cf0269634671e842 636b7123683cd142     0.617733
_S_          1 cf0269634671e842 636b7123683cd142     0.617615
_S_          1 cf0269634671e842 636b7123683cd142     0.618303
_S_          1 cf0269634671e842 636b7123683cd142     0.617685
_S_          1 cf0269634671e842 636b7123683cd142     0.617538
_S_          1 cf0269634671e842 636b7123683cd142     0.617712
_S_          1 cf0269634671e842 636b7123683cd142     0.626201
----------------------------------------------------------------------------------------------------------------------------------------
The tuned version is 2.76X better than the rererence code
========================================================================================================================================
gcc -O3 -mavx2 bench_keccakf_quad.cpp -o bench_keccakf_quad_gcc
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.618130
_S_          1 cf0269634671e842 636b7123683cd142     0.618130
_S_          2 4c2ce1c0a38b54f3 2f4790e3cbb785b1     0.618130
_S_          3 c23846f289232c2d ed7fd6114294a99c     0.618130
_S_          4 82a600ea7029e730 6fd9d6fb32bd4eac     0.610089
_S_          5   75db901707152f 6fac0d6b25ba5b83     0.610089
_S_          6 e69ed015b75d5da0 8932dd7e92e70623     0.610089
_S_          7 b0bf01b096c67f87 398ddcce042179a4     0.610089
_F_ fkey=      0 lkey=      8 loops= 1048576 SUM= 398ddcce042179a4 seconds= 1.228265   Mks= 6.829640
----------------------------------------------------------------------------------------------------------------------------------------
 jmany bench_keccakf_quad_gcc 0 8
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.764634
_S_          1 cf0269634671e842 636b7123683cd142     0.764634
_S_          2 4c2ce1c0a38b54f3 2f4790e3cbb785b1     0.764634
_S_          3 c23846f289232c2d ed7fd6114294a99c     0.764634
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.763445
_S_          1 cf0269634671e842 636b7123683cd142     0.763445
_S_          2 4c2ce1c0a38b54f3 2f4790e3cbb785b1     0.763445
_S_          3 c23846f289232c2d ed7fd6114294a99c     0.763445
....
_S_          4 82a600ea7029e730 6fd9d6fb32bd4eac     0.767377
_S_          5   75db901707152f 6fac0d6b25ba5b83     0.767377
_S_          6 e69ed015b75d5da0 8932dd7e92e70623     0.767377
_S_          7 b0bf01b096c67f87 398ddcce042179a4     0.767377
_F_ fkey=      0 lkey=      8 loops= 1048576 SUM= 398ddcce042179a4 seconds= 1.533158   Mks= 5.471457
_S_          4 82a600ea7029e730 6fd9d6fb32bd4eac     0.767036
_S_          5   75db901707152f 6fac0d6b25ba5b83     0.767036
_S_          6 e69ed015b75d5da0 8932dd7e92e70623     0.767036
_S_          7 b0bf01b096c67f87 398ddcce042179a4     0.767036
_F_ fkey=      0 lkey=      8 loops= 1048576 SUM= 398ddcce042179a4 seconds= 1.534700   Mks= 5.465959
----------------------------------------------------------------------------------------------------------------------------------------
icc -O3 -x=core-avx2 bench_keccakf_quad.cpp -o bench_keccakf_quad_icc
 bench_keccakf_quad_icc 0 8
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.651637
_S_          1 cf0269634671e842 636b7123683cd142     0.651637
_S_          2 4c2ce1c0a38b54f3 2f4790e3cbb785b1     0.651637
_S_          3 c23846f289232c2d ed7fd6114294a99c     0.651637
_S_          4 82a600ea7029e730 6fd9d6fb32bd4eac     0.653566
_S_          5   75db901707152f 6fac0d6b25ba5b83     0.653566
_S_          6 e69ed015b75d5da0 8932dd7e92e70623     0.653566
_S_          7 b0bf01b096c67f87 398ddcce042179a4     0.653566
_F_ fkey=      0 lkey=      8 loops= 1048576 SUM= 398ddcce042179a4 seconds= 1.305248   Mks= 6.426831
----------------------------------------------------------------------------------------------------------------------------------------
The vectorized code compute 4 chains at each call then the performance is the same 
We can see the 4 chains are computed with 30% degradation vs 1 at a time ; this is a significant 3X improvment over the single chain version
----------------------------------------------------------------------------------------------------------------------------------------
 jmany bench_keccakf_quad_icc 0 8
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.813202
_S_          1 cf0269634671e842 636b7123683cd142     0.813202
_S_          2 4c2ce1c0a38b54f3 2f4790e3cbb785b1     0.813202
_S_          3 c23846f289232c2d ed7fd6114294a99c     0.813202
_S_          0 ac6918402e4d3900 ac6918402e4d3900     0.816561
_S_          1 cf0269634671e842 636b7123683cd142     0.816561
_S_          2 4c2ce1c0a38b54f3 2f4790e3cbb785b1     0.816561
.......
_S_          4 82a600ea7029e730 6fd9d6fb32bd4eac     0.820172
_S_          5   75db901707152f 6fac0d6b25ba5b83     0.820172
_S_          6 e69ed015b75d5da0 8932dd7e92e70623     0.820172
_S_          7 b0bf01b096c67f87 398ddcce042179a4     0.820172
_F_ fkey=      0 lkey=      8 loops= 1048576 SUM= 398ddcce042179a4 seconds= 1.638680   Mks= 5.119125
_S_          4 82a600ea7029e730 6fd9d6fb32bd4eac     0.820079
_S_          5   75db901707152f 6fac0d6b25ba5b83     0.820079
_S_          6 e69ed015b75d5da0 8932dd7e92e70623     0.820079
_S_          7 b0bf01b096c67f87 398ddcce042179a4     0.820079
_F_ fkey=      0 lkey=      8 loops= 1048576 SUM= 398ddcce042179a4 seconds= 1.640481   Mks= 5.113505
----------------------------------------------------------------------------------------------------------------------------------------
Using all cores also degrade by 25% the performance vs 1 single core
gcc is slightly better 6% than icc on the quad version
========================================================================================================================================

