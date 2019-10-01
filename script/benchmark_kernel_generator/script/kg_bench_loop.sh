#!/bin/bash
#export PATH=`dirname $0`:$PATH


MAX=8
op=f
operation="mmmmmmmmmmmmmm"
loop=1
for ((i=1; i<=$MAX; i++)); do
    loop=$(($loop * 10))
    printf "%12d  " $loop

    ./bin/tool_kernel_generator/kg -L $loop  -W 128 -O $operation  | grep  "^IPC" | awk '{print $2}'
done








