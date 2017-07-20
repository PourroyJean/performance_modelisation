#!/bin/bash


########################################################################################################################
# This script is used to profile a specific application running on one node
# It has to be in the the folder containing: op_merging program and op_start.sh op_stop.sh op_analysis.sh scripts
########################################################################################################################



if [ ${#@} == 0 ]; then
    echo "Usage: ./op_main.sh PROG"
    echo "* PROG: name of the program you want to profile"
    exit
fi



############################################### CONFIGURATION ##########################################################

#GENERAL
export OP_DIR_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export OP_CURRENT_PATH=`pwd`
export OP_START_SCRIPT="$OP_DIR_PATH/op_start.sh"
export OP_STOP_SCRIPT="$OP_DIR_PATH/op_stop.sh"
export OP_ANALYSIS_SCRIPT="$OP_DIR_PATH/op_analysis.sh"
export OP_LOG=false
export OP_TMP_FILE="/tmp/op_tmp_log"
export OP_PROGRAM=$(readlink -f "$1")      #Name of the program you want to profile

#PROFILING
export OP_SAMPLE_FREQ=10000000
export OP_EVENT_1="cpu_clk_unhalted"
export OP_EVENT_2="inst_retired"
export OP_FILE_SUMMARY="$OP_CURRENT_PATH/report_summary.txt"
export OP_FILE_INST="$OP_CURRENT_PATH/report_instructions.txt"

#Analysis
export OP_MERGING_PROGRAM="$OP_DIR_PATH/op_merging"
export OP_OBJ_FILE="$OP_CURRENT_PATH/op_objdump.txt"
export OP_MERGED_FILE="$OP_CURRENT_PATH/op_merged.txt"

#if the script is being sourced we only export the variable and leave
[[ "${BASH_SOURCE[0]}" != "${0}" ]] && exit

########################################################################################################################



if $OP_LOG ; then
    echo "  ################ CONFIGURATION #################"
    echo "~~ MAIN ~~"
    echo "  * OP_DIR_PATH:        $OP_DIR_PATH"
    echo "  * OP_PROGRAM:         $OP_PROGRAM"

    echo "~~ PROFILING ~~"
    echo "  * OP_SAMPLE_FREQ      $OP_SAMPLE_FREQ"
    echo "  * OP_EVENT_1          $OP_EVENT_1"
    echo "  * OP_EVENT_2          $OP_EVENT_2"
    echo "  * OP_FILE_SUMMARY     $OP_FILE_SUMMARY"
    echo "  * OP_FILE_INST        $OP_FILE_INST"

    echo "~~ ANALYSIS ~~"
    echo "  * OP_MERGING_PROGRAM  $OP_MERGING_PROGRAM"
    echo "  * OP_OBJ_FILE         $OP_OBJ_FILE"
    echo "  * OP_MERGED_FILE      $OP_MERGED_FILE"

fi


printf "\n    ################ START PROFILING #################\n"
printf "Starting ... "
$OP_START_SCRIPT
echo "the profiler is now running..."


printf "\n    ################ EXECUTION #######################\n"
printf "Executing $OP_PROGRAM ... "
$OP_PROGRAM
echo "END"


printf "\n    ################ STOP PROFILING #################\n"
$OP_STOP_SCRIPT
echo "Profiling finished..."


printf "\n    ################ ANALYSIS #################\n"
$OP_ANALYSIS_SCRIPT
