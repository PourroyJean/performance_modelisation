#!/usr/bin/env bash

ls /opt/intel/compilers_and_libraries/linux/bin/compilervars.sh

# Source your Intel MPI Library if not already done
. /opt/intel/compilers_and_libraries/linux/bin/compilervars.sh intel64
export PATH=.:$PATH


# Path of the
check_bin  (){
BIN=$1
if [[ -x "$BIN" ]]
then
    echo "Execution of $BIN"
else
    >&2 echo "Check the path of you binary $BIN"
    exit -1
fi
}



CORE_SOCKET_0=`numactl --hardware | grep "node 0" | head -n 1 | cut --delimiter=' ' -f 4-`






export I_MPI_PIN_PROCESSOR_LIST=0,80 #thread and hyperthread
export I_MPI_PIN_PROCESSOR_LIST=0,79

export I_MPI_PIN_PROCESSOR_LIST="0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19"
export PIN_CORE_LIST="0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99 100 101 102 103 104 105 106 107 108 109 110 111 112 113 114 115 116 117 118 119 120 121 122 123 124 125 126 127 128 129 130 131 132 133 134 135 136 137 138 139 140 141 142 143 144 145 146 147 148 149 150 151 152 153 154 155 156 157 158 159"

#mpirun -np 20 numactl  ./bin/benchmark_dml_mem/dml_mpi --steplog 0.01 --unroll 2 --type read --cacheline 64 --prefix out --output out --matrixsize 200 --minlog 5 --maxlog 6.7

#export I_MPI_PIN_PROCESSOR_LIST=0,20,40,60
#mpirun -np 4 numactl  ./bin/benchmark_dml_mem/dml_mpi --steplog 0.31 --unroll 2 --type read --cacheline 64 --prefix out --output out --matrixsize 200 --minlog 5 --maxlog 6.7





# ---------------------------------
# -- TEST OF COUPLE MPI PROCESS  --
# ---------------------------------
test_mpi_couple (){
    export PIN_CORE_LIST="0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99"
    for core_ref in $PIN_CORE_LIST  ; do
        for core_test in $PIN_CORE_LIST  ; do
            if [[ $core_ref -eq $core_test ]] ; then
                continue
            fi
            export I_MPI_PIN_PROCESSOR_LIST="$core_ref,$core_test"
            printf "$I_MPI_PIN_PROCESSOR_LIST = "
            mpirun -np 4 numactl  ./bin/benchmark_memory/dml_mpi --steplog 0.31 --unroll 2 --type read --cacheline 64 --prefix out --matrixsize 200 --minlog 5 | grep Bandwidth
        done
    done
}





# -------------------------------------------------------
# -- How much core is needed to saturate the bandwidth --
# -------------------------------------------------------
#
#  Nb_Core Band_unit       Band_Total
#  1       12.407164       12.4072         -- Cores used : 0
#  2       12.057465       24.1149         -- Cores used : 0,1
#  3       11.959809       35.8794         -- Cores used : 0,1,2
#  4       11.819736       47.2789         -- Cores used : 0,1,2,3
#
#
test_mpi_bandwidth_saturation (){
    export PIN_CORE_LIST="0 20 1 21 2 22 3 23 4 24 5 25 6 26 7 27 8 28 9 29 10 30"
    export PIN_CORE_LIST="0,20 1,21 2,22 3,23 4,24 5,25 6,26 7,27 8,28 9,29 10,30 11,31 12,32 13,33 14,34 15,35 16,36 17,37 18,38 19,39"
    export PIN_CORE_LIST="0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19"
    #export PIN_CORE_LIST="0 1,2,3,4,5 6,7,8,9"
    #export PIN_CORE_LIST="0,1,2,3,4,5,6,7"
    core_list=
    EXE="./bin/benchmark_dml_mem/dml_mpi"
    check_bin $EXE

    echo -e "Nb_Core\tBand_unit\tBand_Total"
    for core_test in $PIN_CORE_LIST  ; do
        core_list="$core_list$core_test"
        export I_MPI_PIN_PROCESSOR_LIST="$core_list"
        nb_core=$((`echo $core_list | grep -o "," | wc -l` + 1))

        STRIDE=64
        UNROLL=8    #1 is the best in memory
        MINLOG=8.3  #1.5 GiB
        MAXLOG=8.8  #3.0 GiB
        HUGEPAGES="--hugepages"
        HUGEPAGES=""
        mpirun -np $nb_core numactl  $EXE --steplog 0.31 --unroll $UNROLL --type read --mode special --cacheline 64 --prefix out --stride $STRIDE --matrixsize 3400 --minlog $MINLOG --maxlog $MAXLOG --output out > res_tmp
        band=`cat res_tmp | grep Bandwidth | awk '{print $2}'`

        band_tot=`echo $band | awk -v nbc=$nb_core '{ print $1*nbc }'`
        echo -e "${nb_core}\t${band}\t${band_tot}\t\t-- Cores used : $core_list "
        core_list="$core_list,"
    done
}



# -------------------------------------------------------
# --     How much bandwidth with max nb of core        --
# -------------------------------------------------------
test_mpi_bandwidth_saturation_max_nb_core (){
    #export PIN_CORE_LIST="0 1,2,3,4,5 6,7,8,9"
    #export PIN_CORE_LIST="0,1,2,3,4,5,6,7"
    echo "Running test_mpi_bandwidth_saturation_max_nb_core ..."
    export PIN_CORE_LIST="0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19"

    EXE="./bin/benchmark_dml_mem/dml_mpi"
    check_bin $EXE

    echo -e "Nb_Core\tBand_unit\tBand_Total"
    for core_test in $PIN_CORE_LIST  ; do
        core_list="$core_list$core_test"
        export I_MPI_PIN_PROCESSOR_LIST="$core_list"
        nb_core=$((`echo $core_list | grep -o "," | wc -l` + 1))

        STRIDE=64
        UNROLL=8    #1 is the best in memory
        MINLOG=8.3  #1.5 GiB
        MAXLOG=8.8  #3.0 GiB
        HUGEPAGES="--hugepages"
        HUGEPAGES=""
        mpirun -np $nb_core numactl  $EXE --steplog 0.31 --unroll $UNROLL --type read --mode special --cacheline 64 --prefix out --stride $STRIDE --matrixsize 3400 --log 0 --output out > res_tmp
        band=`cat res_tmp | grep Bandwidth | awk '{print $2}'`

        band_tot=`echo $band | awk -v nbc=$nb_core '{ print $1*nbc }'`
        echo -e "${nb_core}\t${band}\t${band_tot}\t\t-- Cores used : $core_list "
        core_list="$core_list,"
    done
}










































# ---------------------------------
# -- TEST OF L3 SCALING          --
# ---------------------------------


test_mpi_L3_scaling (){
#Processors:      ( 0 80 ) ( 1 81 ) ( 2 82 ) ...
    export PIN_CORE_LIST="0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19"
    export PIN_CORE_LIST="0 1 2 3"
#    large_page="--hugepages"  #TO BET SET
    core_list=
    for core_test in $PIN_CORE_LIST  ; do
        core_list="$core_list$core_test"
        export I_MPI_PIN_PROCESSOR_LIST="$core_list"
        nb_core=$((`echo $core_list | grep -o "," | wc -l` + 1))
        printf "($nb_core) = "
        band=`mpirun -np $nb_core numactl  ./bin/benchmark_memory/dml_mpi --steplog 0.10 --unroll 2 --type read --cacheline 64 --prefix out_stride_LONG --stride 64 --output out --matrixsize 100 --measure 5 --log 6.55 $large_page | grep Bandwidth`
        band_tot=`echo $band | awk -v nbc=$nb_core '{ print $2*nbc }'`
        echo "$band  -- Total = $band_tot -- Cores used : $core_list"
        core_list="$core_list,"
    done
}




#Use a uniq size of data set shared by multiple process
test_mpi_L3_scaling_2 (){
#Processors:      ( 0 80 ) ( 1 81 ) ( 2 82 ) ...
    export PIN_CORE_LIST="0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19"
    export PIN_CORE_LIST="0 1 2 3 "
    export PIN_CORE_LIST="0 1,2,3,4 5,6,7,8,9 10,11,12,13,14 15,16,17,18,19"



    core_list=   #set to empty
    mat_size=1 #L3 size

    for core_test in $PIN_CORE_LIST  ; do
        core_list="$core_list$core_test"
        export I_MPI_PIN_PROCESSOR_LIST="$core_list"
        nb_core=$((`echo $core_list | grep -o "," | wc -l` + 1))

        STRIDE=64
        UNROLL=1
        MATRIX_SIZE=3000
        STEP_LOG=0
        HUGEPAGES=
        STRIDES=64 #8,16,32,64,128,256,1024
        MODE=special
        MEASURE=10
        EXE="./bin/benchmark_dml_mem/dml_mpi"

        cmd_chunk="bc -l <<< '$mat_size/$nb_core'"
        CHUNK_PER_PROCESS=`eval $cmd_chunk`

        band=`mpirun -np $nb_core numactl  $EXE --unroll $UNROLL --type read --cacheline 64 --prefix out --stride $STRIDE --mode special --matrixsize $CHUNK_PER_PROCESS --measure $MEASURE --steplog $LOG $HUGEPAGES |  grep "out K" | awk '{printf $5 ; printf "MB ";  printf $7}'`
#        mpirun -np $nb_core numactl  $EXE --unroll $UNROLL --type read --cacheline 64 --prefix out --stride $STRIDE --mode special --matrixsize $CHUNK_PER_PROCESS --measure $MEASURE --steplog $LOG $HUGEPAGES
#        mpirun -np $nb_core numactl  $EXE --unroll $UNROLL --type read --cacheline 64 --prefix out --stride $STRIDE --mode special --matrixsize $mat_size --measure $MEASURE --steplog $LOG $HUGEPAGES
#        echo $res
#        band=`mpirun -np $nb_core numactl  ./bin/benchmark_dml_mem/dml_mpi --type read --cacheline 64 --prefix l3_scaling_2  --output out  --steplog 0 --matrixsize $CHUNK_PER_PROCESS --measure 100 --mode special --unroll 1 --stride 8 $large_page | grep Bandwidth `
#        mpirun -np $nb_core numactl  $EXE --unroll $UNROLL --mode $MODE --type read --cacheline 64 --prefix test_mpi_hierarchy_conflict --stride $STRIDES --minlog $MINLOG --maxlog $MAXLOG --output out --matrixsize $MATRIX_SIZE --steplog $STEP_LOG --measure $MEASURE $HUGEPAGES

        band_tot=`echo $band | awk -v nbc=$nb_core '{ print $2*nbc }'`
#        printf "($nb_core) = "
        printf "$nb_core "
#        echo "$band  -- Total = $band_tot -- Cores used : $core_list  (mat size $CHUNK_PER_PROCESS)"
        echo "$band" | awk '{print $2}'
        core_list="$core_list,"
    done
}


test_mpi_sharing_cache (){
    export PIN_CORE_LIST="0 1,2,3,4 5,6,7,8,9 10,11,12,13,14 15,16,17,18,19"
    export PIN_CORE_LIST="0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19"


    LOG=0  #5 = 780 KiB   3.5 = 24 KiB
    EXE="./bin/benchmark_dml_mem/dml_mpi"
    check_bin $EXE

    STRIDE=64
    UNROLL=8
    MATRIX_SIZE=28
    HUGEPAGES=#"--hugepages"
    STRIDES=64 #8,16,32,64,128,256,1024
    MODE=special
    MEASURE=5000

perf_file="test_L3_perf_log"

    printf  '%-8s %-12s %-13s %-15s %-15s\n' "Nb_core" "Data_set" "LLC_miss" "bandwidth_core"  "bandwidth_total"
    for core_test in $PIN_CORE_LIST  ; do
            core_list="$core_list$core_test"
            export I_MPI_PIN_PROCESSOR_LIST="$core_list"
            nb_core=$((`echo $core_list | grep -o "," | wc -l` + 1))

            cmd_chunk="bc -l <<< '$MATRIX_SIZE/$nb_core'"
            CHUNK_PER_PROCESS=`eval $cmd_chunk`
#            echo $CHUNK_PER_PROCESS
#            mpirun -np $nb_core numactl  $EXE --unroll $UNROLL --type read --cacheline 64 --prefix out --stride $STRIDE --mode special --matrixsize $CHUNK_PER_PROCESS --measure $MEASURE --steplog $LOG $HUGEPAGES
#            res=`perf stat -B -e cache-misses,cycles:u,instructions:u,task-clock:u,dTLB-load-misses,iTLB-load-misses  -o $perf_file mpirun -np $nb_core numactl  $EXE --unroll $UNROLL --type read --cacheline 64 --prefix out --stride $STRIDE --mode special --matrixsize $CHUNK_PER_PROCESS --measure $MEASURE --steplog $LOG $HUGEPAGES |  grep "out K" | awk '{printf $5 ; printf "MB ";  printf $7}'`
            res=`perf stat -B -e cache-misses,cycles:u,instructions:u,task-clock:u,dTLB-load-misses,iTLB-load-misses  -o $perf_file mpirun -np $nb_core numactl  $EXE --unroll $UNROLL --type read --cacheline 64 --prefix out --stride $STRIDE --mode special --matrixsize $CHUNK_PER_PROCESS --measure $MEASURE --steplog $LOG $HUGEPAGES |  grep "out K" | awk '{printf $5 ; printf "MB ";  printf $7}'`
#            instructions=`cat  $perf_file | grep instructions:u |  awk '{print $1}' | tr -d ,`
#            ipc=`cat  $perf_file | grep instructions:u |  awk '{print $4}'`
            misses=`cat  $perf_file | grep cache-misses |  awk '{print $1}' | tr -d ,`
#            dtlb_misses=`cat  $perf_file | grep dTLB-load-misses |  awk '{print $1}' | tr -d ,`
            size=`echo $res | awk '{print $1}' | tr -d MB `
            band=`echo $res | awk '{print $2}' `
            band_tot=`echo $band | awk -v nbc=$nb_core '{ print $1*nbc }'`
            printf  '%-8s %-12s %-13s %-15s %-15s\n' $nb_core $size $instructions $ipc $misses $dtlb_misses $band $band_tot
              core_list="$core_list,"
    done

}


# ---- USAGE ---
# bench a specific part of the memory hierarchy (L1, L2, L3)
# size      Nb_Core       bandwidth
# 382.4MiB        1       72.278530
# 382.4MiB        2       71.236745
# 382.4MiB        3       73.179540
# 382.4MiB        4       75.607571
# 382.4MiB        5       80.866200
# 382.4MiB        6       84.721498
# 382.4MiB        7       85.625918

test_mpi_hierarchy_specific_bench (){
#Processors:      ( 0 80 ) ( 1 81 ) ( 2 82 ) ...
    export PIN_CORE_LIST="0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19"
    large_page="--hugepages"  #TO BET SET
    large_page=" "  #TO BET SET
    core_list=   #set to empty
    TOTAL_AVAILABLE=15 #best performance
    TOTAL_AVAILABLE=0.8 #L3 size

    echo -e "size\tNb_Core\tbandwidth\tbandtot"

    for core_test in $PIN_CORE_LIST  ; do
        core_list="$core_list$core_test"
        export I_MPI_PIN_PROCESSOR_LIST="$core_list"
        nb_core=$((`echo $core_list | grep -o "," | wc -l` + 1))

        cmd_chunk="bc -l <<< '$TOTAL_AVAILABLE/$nb_core'"

#        -- L1 --
        CHUNK_PER_PROCESS=0.9
        LOG=0 #31 KiB
        UNROLL=1

#        -- L2 --
        CHUNK_PER_PROCESS=30
        LOG=5   #781.0 KiB
        UNROLL=1

#        -- L3 --
        CHUNK_PER_PROCESS=30
        LOG=6.5 #24.1 MiB
        UNROLL=1

#        -- MEMORY --
        CHUNK_PER_PROCESS=1000
        LOG=8.1 #7.7 =382.4 MiB #8.1 = 960 MIB
        UNROLL=1
        STRIDE=64

        mpirun -np $nb_core numactl  ./bin/benchmark_memory/dml_mpi --type read --cacheline 64 --prefix test_mpi_cache_scaling  --output out  --steplog $LOG --matrixsize $CHUNK_PER_PROCESS --measure 100 --mode special --unroll $UNROLL --stride $STRIDE $large_page > res_tmp
        band=`cat res_tmp | grep Bandwidth | awk '{print $2}'`
        data_size=`cat res_tmp | grep "_ test_mpi_cache_scaling K =" | awk '{print $5 $6}'`


#        -- AFFICHAGE --

        band_tot=`echo $band | awk -v nbc=$nb_core '{ print $1*nbc }'`
        echo -e "${data_size}\t${nb_core}\t${band} \t${band_tot} -- Cores used : $core_list  (mat size $CHUNK_PER_PROCESS)"
#        exit


#       mpirun -np $nb_core numactl  ./bin/benchmark_dml_mem/dml_mpi --type read --cacheline 64 --prefix out_stride_LONG  --output out  --steplog 0.01 --minlog 5 --matrixsize 15 --measure 20 --mode special --unroll 16 --stride  8
#        printf "($nb_core) = "
#        echo "$band  -- Total = $band_tot -- Cores used : $core_list  (mat size $CHUNK_PER_PROCESS)"
        core_list="$core_list,"
    done
}


# Test used to prove that L1 and L2 performance are not impacted between cores
test_mpi_hierarchy_conflict (){
#    export PIN_CORE_LIST="0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19"

    # -- Test 1 core, or 20 cores
    export PIN_CORE_LIST="0"
    export PIN_CORE_LIST="0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19"

    STRIDE=64
    UNROLL=1
    MINLOG=8    #8.3 = 1.5 GiB
    MAXLOG=10  #8.6 3.0 GiB
    MATRIX_SIZE=3000
    STEP_LOG=0.01
    HUGEPAGES=
    STRIDES=64 #8,16,32,64,128,256,1024
    MODE=special
    MEASURE=1


    for core_test in $PIN_CORE_LIST  ; do
        core_list="$core_list$core_test"
        export I_MPI_PIN_PROCESSOR_LIST="$core_list"
        nb_core=$((`echo $core_list | grep -o "," | wc -l` + 1))
        printf "($nb_core) = "
#           mpirun -np $nb_core numactl  ./bin/benchmark_dml_mem/dml_mpi --unroll $UNROLL --mode $MODE --type read --cacheline 64 --prefix test_mpi_hierarchy_conflict --stride $STRIDES --minlog $MINLOG --maxlog $MAXLOG --output out --matrixsize $MATRIX_SIZE --steplog $STEP_LOG --measure $MEASURE $HUGEPAGES
#        continue
        band=`mpirun -np $nb_core numactl  ./bin/benchmark_memory/dml_mpi --unroll $UNROLL --mode $MODE --type read --cacheline 64 --prefix test_mpi_hierarchy_conflict --stride $STRIDES --minlog $MINLOG --maxlog $MAXLOG --output out --matrixsize $MATRIX_SIZE --steplog $STEP_LOG --measure $MEASURE $HUGEPAGES | grep Bandwidth`
        band_tot=`echo $band | awk -v nbc=$nb_core '{ print $2*nbc }'`
        echo "$band  -- Total = $band_tot -- Cores used : $I_MPI_PIN_PROCESSOR_LIST"
        core_list="$core_list,"
    done
}



#Nb_core  Data_set     nb_inst       IPC          LLC miss        dtlb_miss    bandwidth_core   bandwidth_total
#1        2.0          265711441     0.23         37324599        167488       12.15            12.15
#2        2.0          521315397     0.22         77204495        348727       11.84            23.68
#3        2.0          772607323     0.21         120451323       535181       11.65            34.95
#4        2.0          1652209634    0.32         167157535       713829       11.49            45.96
#5        2.0          2709002229    0.29         217764439       894819       11.18            55.9
#6        2.0          4936839736    0.31         266595033       1070054      9.85             59.1
test_mpi_scaling_core_or_dataset (){
#    export PIN_CORE_LIST="0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19"

    # -- Test 1 core, or 20 cores
    export PIN_CORE_LIST="0"
    export PIN_CORE_LIST="0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19"
    export PIN_CORE_LIST="0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19"

    LOG=0  #5 = 780 KiB   3.5 = 24 KiB
    EXE="./bin/benchmark_dml_mem/dml_mpi"
    check_bin $EXE

    STRIDE=64
    UNROLL=8
    MATRIX_SIZE=3000
    HUGEPAGES=#"--hugepages"
    STRIDES=64 #8,16,32,64,128,256,1024
    MODE=special
    MEASURE=1
    MAT_SIZE_LIST=`seq 2100 900 2300` #    6.0 == 7.6 MiB  and 6.8 == 48.1 MiB


    printf  '%-8s %-12s %-13s %-12s %-15s %-12s %-16s %-17s\n' "Nb_core" "Data_set" "nb_inst" "IPC" "LLC miss" "dtlb_miss" "bandwidth_core"  "bandwidth_total"
    for core_test in $PIN_CORE_LIST  ; do
            core_list="$core_list$core_test"
            export I_MPI_PIN_PROCESSOR_LIST="$core_list"
            nb_core=$((`echo $core_list | grep -o "," | wc -l` + 1))

        for mat_size in $MAT_SIZE_LIST  ; do

            perf_file="test_L3_perf_log"
            res=`perf stat -B -e cache-misses,cycles:u,instructions:u,task-clock:u,dTLB-load-misses,iTLB-load-misses  -o $perf_file mpirun -np $nb_core numactl  $EXE --unroll $UNROLL --type read --cacheline 64 --prefix out --stride $STRIDE --mode special --matrixsize $mat_size --measure $MEASURE --steplog $LOG $HUGEPAGES |  grep "out K" | awk '{printf $5 ; printf "MB ";  printf $7}'`
#            res=`(set -x; perf stat -B -e cache-misses,cycles:u,instructions:u,task-clock:u  -o $perf_file mpirun -np $nb_core numactl  ./bin/benchmark_dml_mem/dml_mpi --unroll $UNROLL --type read --cacheline 64 --prefix out --stride $STRIDE --mode special --matrixsize $mat_size --measure $MEASURE --steplog $LOG)`
            instructions=`cat  $perf_file | grep instructions:u |  awk '{print $1}' | tr -d ,`
            ipc=`cat  $perf_file | grep instructions:u |  awk '{print $4}'`
            misses=`cat  $perf_file | grep cache-misses |  awk '{print $1}' | tr -d ,`
            dtlb_misses=`cat  $perf_file | grep dTLB-load-misses |  awk '{print $1}' | tr -d ,`
            size=`echo $res | awk '{print $1}' | tr -d MB `
            band=`echo $res | awk '{print $2}' `
            band_tot=`echo $band | awk -v nbc=$nb_core '{ print $1*nbc }'`
            printf  '%-8s %-12s %-13s %-12s %-15s %-12s %-16s %-17s\n' $nb_core $size $instructions $ipc $misses $dtlb_misses $band $band_tot
      done
              core_list="$core_list,"
    done



}



 test_miss (){
#Processors:      ( 0 80 ) ( 1 81 ) ( 2 82 ) ...
    export PIN_CORE_LIST="0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19"
    export PIN_CORE_LIST="0,1,2,3,4"

#    large_page="--hugepages"  #TO BET SET
    large_page=""  #TO BET SET

    STRIDE=64
    UNROLL=1    #1 is the best in memory
    LOG_LIST=`seq 6.0 0.01 6.8` #    6.0 == 7.6 MiB  and 6.8 == 48.1 MiB
    LOG_STEP=0.01
    HUGEPAGES=

    printf  '%-12s %-12s %-13s \n' "Data_set" "Bandwidth" "Cache-misses"


    for log in $LOG_LIST  ; do
#        printf "($log) = "
        perf_file="test_L3_perf_log"


        res=`perf stat -B -e cache-misses -o $perf_file  mpirun -np 5 numactl ./bin/benchmark_memory/dml_mpi --steplog $LOG_STEP --unroll 2 --type read --cacheline 64 --prefix out --stride $STRIDE --matrixsize 300 --measure 5 --log $log $large_page |  grep "out K" | awk '{printf $5 ; printf "MB ";  printf $7}'`
        misses=`cat  $perf_file | grep cache-misses |  awk '{print $1}'`
        size=`echo $res | awk '{print $1}' | tr -d MB `
        band=`echo $res | awk '{print $2}' `
        misses=`echo $misses | tr -d ,`
        printf  '%-12s %-12s %-12s \n' $size $band $misses
#        band_tot=`echo $band | awk -v nbc=$nb_core '{ print $2*nbc }'`
#        echo "$band  -- Total = $band_tot -- Cores used : $core_list"
    done
}



test_jean (){
#Processors:      ( 0 80 ) ( 1 81 ) ( 2 82 ) ...
    export PIN_CORE_LIST="0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19"
    export PIN_CORE_LIST="0 18 11 4 6 5 7 10 19 13 14 8 15 17 3 2 1 9 16 12"
    core_list=
    for core_test in $PIN_CORE_LIST  ; do
        core_list="$core_list$core_test"
        export I_MPI_PIN_PROCESSOR_LIST="$core_list"
        nb_core=$((`echo $core_list | grep -o "," | wc -l` + 1))
        printf "($nb_core) = "

#        band=`(set -x; mpirun -np $nb_core numactl  ./bin/benchmark_dml_mem/dml_mpi --unroll 2 --type read --cacheline 64 --prefix out_stride_LONG --stride 64 --output out --matrixsize 1000 --measure 4 --log 8.11 | grep Bandwidth)`
        band=`mpirun -np $nb_core numactl  ./bin/benchmark_memory/dml_mpi --unroll 2 --type read --cacheline 64 --prefix out_stride_LONG --stride 64 --output out --matrixsize 1000 --measure 4 --log 8.11 | grep Bandwidth`
        band_tot=`echo $band | awk -v nbc=$nb_core '{ print $2*nbc }'`
        echo "$band  -- Total = $band_tot -- Cores used : $I_MPI_PIN_PROCESSOR_LIST"
        core_list="$core_list,"
    done
}



test_prefetch_particular_stride (){
    STRIDE="71760,74288,77168,85936,86512,92656,97424"
    UNROLL=1    #1 is the best in memory
    LOG_STEP=0.10
    HUGEPAGES=
     ./bin/benchmark_memory/dml --steplog $LOG_STEP --unroll $UNROLL --type read --cacheline 64 --stride $STRIDE --output out --measure 1000 --matrixsize 50
}



# -------------------------------------------------------
# -- TEst of strides that have really poor performance --
# -------------------------------------------------------
#Data_set     stride        bandwidth    nb_inst         IPC         misses
#492.6        73704         79.02        68966329       0.51         4250671
#492.6        77816         98.83        68914044       0.55         3864789
#492.6        81928         100.41       69007697       0.52         3062615
test_particular_stride () {

UNROLL=1    #1 is the best in memory
    HUGEPAGES=
    UNROLL_LIST="1 2 4 8 16 32 64"
    STRIDES_GOOD="73704 77816 81928 86008 90136 94224 98312"
    STRIDES_BAD="73728 77824 81920 86016 90112 94208 98304"
    STRIDES="$STRIDES_GOOD $STRIDES_BAD"
    STRIDES="73704 73728 77816 77824 81928 81920 86008 86016 90136 90112 94224 94208 98312 98304"
    UNROLL=8
    LOG=0  #5 = 780 KiB   3.5 = 24 KiB
    HUGEPAGES=
    MODE=special
    MEASURE=10
    MAT_SIZE=10000
    EXE="./bin/benchmark_dml_mem/dml"
    check_bin $EXE

    printf  '%-12s %-13s %-12s %-15s %-11s %-12s\n' "Data_set" "stride" "bandwidth" "nb_inst" "IPC" "misses"

    for STRIDE in $STRIDES  ; do
        perf_file="test_L3_perf_log"
        res=`perf stat -B -e cache-misses,cycles:u,instructions:u,task-clock:u  -o $perf_file ./$EXE --unroll $UNROLL --type read --cacheline 64 --prefix out --stride $STRIDE --mode special --matrixsize $MAT_SIZE --measure $MEASURE --steplog $LOG |  grep "out K" | awk '{printf $5 ; printf "MB ";  printf $7}'`
        instructions=`cat  $perf_file | grep instructions:u |  awk '{print $1}'`
        ipc=`cat  $perf_file | grep instructions:u |  awk '{print $4}'`
        misses=`cat  $perf_file | grep misses |  awk '{print $1}' | tr -d ,`
        instructions=`echo $instructions | tr -d ,`
        size=`echo $res | awk '{print $1}' | tr -d MB `
        band=`echo $res | awk '{print $2}' `
        printf  '%-12s %-12s  %-12s %-14s %-12s %-12s\n' $size $STRIDE $band $instructions $ipc $misses
    done
}






test_unrolling (){
    UNROLL_LIST="1 2 4 8 16 32 64"
    STRIDE=8
    UNROLL=16
    LOG=3.5  #5 = 780 KiB, 3.5 = 24 KiB
    HUGEPAGES=
    MODE=special
    MEASURE=1000
    MAT_SIZE=2
#    EXE="./bin/benchmark_dml_mem/dml_icc_no_unroll"
    EXE=$BIN
    check_bin $EXE

    printf  '%-12s %-13s %-12s %-15s %-12s \n' "Data_set" "unrolling" "bandwidth" "nb_inst" "IPC"

    for UNROLL in $UNROLL_LIST  ; do
#        printf "($log) = "
        perf_file="test_L3_perf_log"


#        res=`perf stat -B -e cache-misses -o $perf_file ./bin/benchmark_dml_mem/dml_icc_no_unroll --unroll $UNROLL --type read --cacheline 64 --prefix out --stride $STRIDES --matrixsize 2 --measure 100 --log $log $large_page |  grep "out K" | awk '{printf $5 ; printf "MB ";  printf $7}'`
#        res=`./bin/benchmark_dml_mem/dml_icc_no_unroll --unroll $UNROLL --type read --cacheline 64 --prefix out --stride $STRIDES --matrixsize 2 --measure 10 --log $log $large_page |  grep "Bandwidth"`
        res=`perf stat -B -e cycles:u,instructions:u,task-clock:u  -o $perf_file ./$EXE --unroll $UNROLL --type read --cacheline 64 --prefix out --stride $STRIDE --mode special --matrixsize $MAT_SIZE --measure $MEASURE --log $LOG |  grep "out K" | awk '{printf $5 ; printf "MB ";  printf $7}'`
#        echo $res
#        cat $perf_file
#        exit
        instructions=`cat  $perf_file | grep instructions:u |  awk '{print $1}'`
        ipc=`cat  $perf_file | grep instructions:u |  awk '{print $4}'`
        instructions=`echo $instructions | tr -d ,`
        size=`echo $res | awk '{print $1}' | tr -d MB `
        band=`echo $res | awk '{print $2}' `
        printf  '%-12s %-12s  %-12s %-14s %-12s\n' $size $UNROLL $band $instructions $ipc
#        band_tot=`echo $band | awk -v nbc=$nb_core '{ print $2*nbc }'`
#        echo "$band  -- Total = $band_tot -- Cores used : $core_list"
    done



}


#test_bw_staturation
#test_mpi_bandwidth_saturation
test_mpi_bandwidth_saturation_max_nb_core
#test_mpi_L3_scaling
#test_mpi_hierarchy_conflict
#test_mpi_scaling_core_or_dataset
#test_mpi_hierarchy_specific_bench
#test_mpi_L3_scaling_2
#test_mpi_sharing_cache
#test_mpi_cache_scaling
#test_bw_staturation
#test_jean
#test_miss
#test_unrolling
#test_prefetch_particular_stride
#test_particular_stride
#test_unrolling_special







