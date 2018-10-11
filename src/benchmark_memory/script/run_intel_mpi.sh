#!/usr/bin/env bash

ls /opt/intel/compilers_and_libraries/linux/bin/compilervars.sh

# Source your Intel MPI Library if not already done
. /opt/intel/compilers_and_libraries/linux/bin/compilervars.sh intel64
export PATH=.:$PATH

# Path of the
BIN=/nfs/pourroy/code/THESE/performance_modelisation/build/bin/benchmark_memory/bm_mpi

if [[ -x "$BIN" ]]
then
    echo "Execution of $BIN"
else
    echo "Check the path of you binary $BIN"
fi




CORE_SOCKET_0=`numactl --hardware | grep "node 0" | head -n 1 | cut --delimiter=' ' -f 4-`






export I_MPI_PIN_PROCESSOR_LIST=0,80 #thread and hyperthread
export I_MPI_PIN_PROCESSOR_LIST=0,79

#export I_MPI_PIN_PROCESSOR_LIST="0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19"
#mpirun -np 20 numactl  ./bin/benchmark_memory/bm_mpi --steplog 0.01 --unroll 2 --type read --cacheline 64 --prefix out --output out --matrixsize 200 --minlog 5 --maxlog 6.7

#export I_MPI_PIN_PROCESSOR_LIST=0,20,40,60
#mpirun -np 4 numactl  ./bin/benchmark_memory/bm_mpi --steplog 0.31 --unroll 2 --type read --cacheline 64 --prefix out --output out --matrixsize 200 --minlog 5 --maxlog 6.7




# ---------------------------------
# -- TEST OF COUPLE MPI PROCESS  --
# ---------------------------------
#export PIN_CORE_LIST="0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99"
#for core_ref in $PIN_CORE_LIST  ; do
#    for core_test in $PIN_CORE_LIST  ; do
#        if [[ $core_ref -eq $core_test ]] ; then
#            continue
#        fi
#        export I_MPI_PIN_PROCESSOR_LIST="$core_ref,$core_test"
#        printf "$I_MPI_PIN_PROCESSOR_LIST = "
#        mpirun -np 4 numactl  ./bin/benchmark_memory/bm_mpi --steplog 0.31 --unroll 2 --type read --cacheline 64 --prefix out --matrixsize 200 --minlog 5 | grep Bandwidth
#    done
#done


# ---------------------------------
# -- TEST OF ONE PROC SCALING  --
# ---------------------------------
export PIN_CORE_LIST="0 20 1 21 2 22 3 23 4 24 5 25 6 26 7 27 8 28 9 29 10 30"
export PIN_CORE_LIST="0,20 1,21 2,22 3,23 4,24 5,25 6,26 7,27 8,28 9,29 10,30 11,31 12,32 13,33 14,34 15,35 16,36 17,37 18,38 19,39"
export PIN_CORE_LIST="0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19"
#export PIN_CORE_LIST="0 1,2,3,4,5 6,7,8,9"
#export PIN_CORE_LIST="0,1,2,3,4,5,6,7"
core_list=
for core_test in $PIN_CORE_LIST  ; do
    core_list="$core_list$core_test"
    export I_MPI_PIN_PROCESSOR_LIST="$core_list"
    nb_core=$((`echo $core_list | grep -o "," | wc -l` + 1))
    printf "($nb_core) = "
    band=`mpirun -np $nb_core numactl  ./bin/benchmark_memory/bm_mpi --steplog 0.31 --unroll 2 --type read --cacheline 64 --prefix out --stride 64 --matrixsize 3400 --minlog 8.3 --maxlog 8.8 --output out | grep Bandwidth`
    band_tot=`echo $band | awk -v nbc=$nb_core '{ print $2*nbc }'`
    echo "$band  -- Total = $band_tot -- Cores used : $core_list"
    core_list="$core_list,"
#    read
done