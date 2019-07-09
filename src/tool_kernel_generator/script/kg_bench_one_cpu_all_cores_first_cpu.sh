#!/bin/bash
#export PATH=`dirname $0`:$PATH

BENCH="assembly"

# -- WILL LAUNCH THE BENCH ON CPUs : --
#CPU 0, 1, 2 .... NB_CORES_TO_BENCH
NB_CORES_TO_BENCH=`lscpu | grep "Core(s)" | awk '{print $4}'` #TOTAL


#  -- FIND THE FIRST CORE OF THE SECOND SOCKET --
#  numactl --hardware
#  node 0 cpus: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53
#  node 1 cpus: 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71
NEXT_SOCKET=`numactl --hardware | grep "node 1" | head -n 1 | awk '{print $4}'`


CORE_HARDWARE=`numactl --hardware | grep "node 0 cpus" | awk '{for(i=4;i<=NF;++i)printf $i " "}'`


arr=($CORE_HARDWARE)
CORE_LIST=(${arr[@]:0:($NB_CORES_TO_BENCH)})



if [[ $NB_CORES_TO_BENCH -gt ${#CORE_LIST[@]} ]]; then
    echo "Too much core to bench ($NB_CORES_TO_BENCH) for only one CPU (max ${#CORE_LIST[@]}) ..."
    exit
fi


echo  "Launching $BENCH on {${CORE_LIST[*]}}"


for core in "${CORE_LIST[@]}" ; do
     if [[ "$core" == ${CORE_LIST[-1]} ]]; then
        ./$BENCH $core
        continue
    fi
     ./$BENCH $core  | grep "Running" &

done


