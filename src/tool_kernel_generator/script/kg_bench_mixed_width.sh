#!/bin/bash
#export PATH=`dirname $0`:$PATH


BENCH_LIST="assembly_128_scalar  assembly_256_scalar assembly_256_128  assembly_512_scalar assembly_512_128  assembly_512_256  "




for bench in $BENCH_LIST; do
    printf "%-20s %s" $bench
    ./$bench | grep _value_ | awk '{print $6}'
done











