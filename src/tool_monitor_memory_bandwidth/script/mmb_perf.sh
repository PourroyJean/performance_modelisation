#!/bin/bash
#export PATH=`dirname $0`:$PATH


while true ; do

echo a ;
sleep .5;
done





       Some PMUs are not associated with a core, but with a whole CPU socket. Events on these PMUs generally cannot be sampled, but only counted globally with perf stat -a. They can be bound to one logical CPU, but will measure all the CPUs in the same
       socket.

       This example measures memory bandwidth every second on the first memory controller on socket 0 of a Intel Xeon system

           perf stat -C 0 -a uncore_imc_0/cas_count_read/,uncore_imc_0/cas_count_write/ -I 1000 ...

       Each memory controller has its own PMU. Measuring the complete system bandwidth would require specifying all imc PMUs (see perf list output), and adding the values together.

