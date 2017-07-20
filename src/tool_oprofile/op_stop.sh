#!/bin/bash





############################################### CONFIGURATION ##########################################################
#Configuration is done in op_main.sh
########################################################################################################################


sudo opcontrol --dump
#Generating two report
opreport    1> $OP_FILE_SUMMARY 2> $OP_TMP_FILE
opreport -d 1> $OP_FILE_INST    2> $OP_TMP_FILE




if $OP_LOG ; then
    cat $OP_TMP_FILE
fi

