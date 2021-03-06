#!/bin/bash





############################################### CONFIGURATION ##########################################################
#Configuration is done in op_main.sh
########################################################################################################################


echo   "-----------------------------------------------"
echo   "---------- REPORT SUMMARY ---------------------"
echo   "-----------------------------------------------"
head -n 11 $OP_FILE_SUMMARY | tail -n 9
echo
echo   "-----------------------------------------------"
echo   "---------- REPORT INSTRUCTIONS ----------------"
echo   "-----------------------------------------------"

grep -v "^ " $OP_FILE_INST | sed -n 4,9p

echo   "-----------------------------------------------"
echo   "---------- MERGE: OPROFILE + OBJDUMP ----------"
echo   "-----------------------------------------------"

objdump -d  $OP_PROGRAM  > $OP_OBJ_FILE
$OP_MERGING_PROGRAM --object $OP_OBJ_FILE  --profile $OP_FILE_INST > $OP_MERGED_FILE
cat op_merged.txt | grep IPC

