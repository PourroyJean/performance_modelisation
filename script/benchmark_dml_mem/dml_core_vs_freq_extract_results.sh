#!/bin/bash

param=${@:1}


dir=""


#       USAGE
# ./dml_core_vs_freq_extract_results.sh ../../results/benchmark_dml_mem/core_vs_freq/work.*
#


for f in *$param*; do [[ -f "$f" ]] || dir="$dir $f"; done



res_file=RES_FILE_CORE_VS_FREQ
rm $res_file
#echo $dir[1]

for d in $param;do
    echo $d

    #Parsing directory name: work.9Px1T.jobid-65877.o184i027.20190927.211841..6148.9.MPI.2.9.GHz
    res_nb_core=`echo $d | awk '{split($0,a,"."); print a[13]}'`
    res_freq=`echo $d | awk '{split($0,a,"."); print a[15]}'`.`echo $d | awk  '{split($0,a,"."); print a[16]}'`
    res_bw=`cat $d/out_DATA | tail -n 1 | awk -v core=$res_nb_core '{split($0,a,",");print a[2]*core}'`

    echo "$res_nb_core $res_freq $res_bw "  >> $res_file
done

exit
