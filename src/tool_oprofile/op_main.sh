#!/bin/bash


########################################################################################################################
# This script is used to profile a specific application running on one node
# IT has to be in the same folder as objdump_parsing program and op_start.sh and op_stop.sh script
########################################################################################################################



if [ ${#@} == 0 ]; then
    echo "Usage: $0 PROG"
    echo "* PROG: name of the program you want to profile"
    exit
fi




############################################### CONFIGURATION ##########################################################
export OP_DIR_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export OP_CURRENT_PATH=`pwd`
export OP_START_SCRIPT=$OP_DIR_PATH/op_start.sh
export OP_STOP_SCRIPT=$OP_DIR_PATH/op_stop.sh

#GENERAL
export OP_LOG=true
export OP_TMP_FILE="/tmp/op_tmp_log"
export OP_PROGRAM=$(readlink -f "$1")      #Name of the program you want to profile
#START
export OP_SAMPLE_FREQ=10000000
export OP_EVENT_1="cpu_clk_unhalted"
export OP_EVENT_2="inst_retired"
#STOP
export OP_FILE_SUMMARY="$OP_CURRENT_PATH/report_summary.txt"
export OP_FILE_INST="$OP_CURRENT_PATH/report_instructions.txt"
#
export OP_MERGING_PROGRAM="$OP_DIR_PATH/op_merging"
export OP_OBJ_FILE="$OP_CURRENT_PATH/op_objdump.txt"
export OP_MERGED_FILE="$OP_CURRENT_PATH/op_merged.txt"


########################################################################################################################

if $OP_LOG ; then
    echo "  ################ CONFIGURATION #################"
    echo "~~ MAIN ~~"
    echo "  * OP_TOOL_PATH:        $OP_TOOL_PATH"
    echo "  * MONITORED PROGRAM:   $OP_PROGRAM"

    echo "~~ START PROFILING ~~"
    echo "  * OP_SAMPLE_FREQ      $OP_SAMPLE_FREQ"
    echo "  * OP_EVENT_1          $OP_EVENT_1"
    echo "  * OP_EVENT_2          $OP_EVENT_2"

    echo "~~ STOP PROFILING ~~"
    echo "  * OP_FILE_SUMMARY     $OP_FILE_SUMMARY"
    echo "  * OP_FILE_INST        $OP_FILE_INST"

    echo "~~ ANALYSIS ~~"
    echo "  * OP_MERGING_PROGRAM  $OP_MERGING_PROGRAM"
    echo "  * OP_OBJ_FILE         $OP_OBJ_FILE"
    echo "  * OP_MERGED_FILE      $OP_MERGED_FILE"

fi


printf "\n    ################ START PROFILING #################\n"
echo "Starting..."
$OP_START_SCRIPT
echo "... the profiler is now running..."


printf "\n    ################ EXECUTION #######################\n"
printf "Executing $OP_PROGRAM... "
$OP_PROGRAM
echo " ... end"


printf "\n    ################ STOP PROFILING #################\n"
$OP_STOP_SCRIPT
echo "Profiling finished..."


printf "\n    ################ ANALYSIS #################\n"
echo   "-----------------------------------------------"
echo   "---------- REPORT SUMMARY ---------------------"
echo   "-----------------------------------------------"
head -n 10 $OP_FILE_SUMMARY | tail -n 7
echo
echo   "-----------------------------------------------"
echo   "---------- REPORT INSTRUCTIONS ----------------"
echo   "-----------------------------------------------"

grep -v "^ " $OP_FILE_INST | sed -n 4,10p

echo   "-----------------------------------------------"
echo   "---------- MERGE: OPROFILE + OBJDUMP ----------"
echo   "-----------------------------------------------"

objdump -d  $OP_PROGRAM  > $OP_OBJ_FILE
$OP_MERGING_PROGRAM $OP_OBJ_FILE $OP_FILE_INST > $OP_MERGED_FILE
cat op_merged.txt | grep IPC

