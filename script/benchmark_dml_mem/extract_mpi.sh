#!/bin/bash

param=${@:1}


dir=""


for f in *$param*; do [[ -f "$f" ]] || dir="$dir $f"; done

res_file=RES_FILE
#echo $dir[1]

for d in $dir;do
    tail -n +2 $d/out_DATA  | awk -F "," '{print $1}' > $res_file
    break
done

cat $res_file



for d in $dir;do
   output_file="$d/out_DATA"
  echo  "---- $d ---- "
  if [ `grep -c '#This was launch with' $output_file` -eq 0 ]; then
    echo "Running..."
  elif [ `grep -c 'This was launch with' $output_file` -ge 1 ]; then  ### Use here a test that validates the run
#    grep -A1 'Total wall-clock time:' $d/myoutput
    #grep -A2 'Summary Results' $d/*res
    tail -n +2 $output_file  | awk -F "," '{print $2}' > tmp_res
    paste $res_file tmp_res > tmp
    mv tmp $res_file
  else
    echo 'FAILED'
  fi

done


