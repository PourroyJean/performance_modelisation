#!/bin/bash
export PATH=`dirname $0`:$PATH

echo $PATH

echo Running test in parallel on CPUs: $*
for i in $*
do
  /nfs/pourroy/code/THESE/performance_modelisation/build/bin/tool_freq $i  &
done
wait
