#!/bin/bash





############################################### CONFIGURATION ##########################################################
#Configuration is done in op_main.sh
########################################################################################################################


#Case check: Intel changed the syntax depending on the processor (example cpu_clk_unhalted or CPU_CLK_UNHALTED)
OP_EVENT_1=`sudo opcontrol -l | grep -i $OP_EVENT_1 |  head -n 1 |  awk '{print $1}' | tr -d :`
OP_EVENT_2=`sudo opcontrol -l | grep -i $OP_EVENT_2 |  head -n 1 |  awk '{print $1}' | tr -d :`

if $OP_LOG ; then
    echo "List of events profiled:"
    echo "  - $OP_EVENT_1"
    echo "  - $OP_EVENT_2"
    echo "Sampling frequency:"
    echo "  - $OP_SAMPLE_FREQ"
fi


########################################################################################################################

sudo opcontrol --shutdown           &> $OP_TMP_FILE
sudo opcontrol --init               &>> $OP_TMP_FILE
sudo opcontrol --reset              &>> $OP_TMP_FILE
sudo opcontrol --setup --no-vmlinux   --event=$OP_EVENT_1:$OP_SAMPLE_FREQ --event=$OP_EVENT_2:$OP_SAMPLE_FREQ &>> $OP_TMP_FILE
sudo opcontrol --save=junk          &>> $OP_TMP_FILE
sudo opcontrol --start              &>> $OP_TMP_FILE


if $OP_LOG ; then
    cat $OP_TMP_FILE
fi