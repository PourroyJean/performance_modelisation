#!/bin/bash

#export PATH=`dirname $0`:$PATH

EXE=$1
echo $EXE

if [[ ! -x "$EXE" ]]
then
    echo "Executable not found or not executable: $*"
fi



#-- AVX BASE
./$EXE



exit

echo Running test in parallel on CPUs: $*
for i in $*
do
  /nfs/pourroy/code/THESE/performance_modelisation/build/bin/tool_freq $i  &
done
wait
