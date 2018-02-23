#!/usr/bin/env bash

#!/bin/bash


# ---- SETTINGS ----
MSR_TOOS_DIR="/nfs/pourroy/code/THESE/msr-tools"



# ---- COMMAND ----
#PERF_ON="true"
#PRINT_PERFEVTSEL="true"


#SETUP_PMC="true"
#RESET_PMC="true"
#ACTIVATE_PMC="true"


#RESET_FIXED_FUNCTION="true"
#ACTIVATE_FIXED_FUNCTION="true"
#READ_FIXED_FUNCTION="true"

# ---- VARIABLE ----
export NCORES=`cat /proc/cpuinfo | grep -c processor`
echo "Number of cores is $NCORES"
export MAXCORE=`expr $NCORES - 1`
export MAXCORE=0



# ---- MAIN ----
if [ "$PERF_ON" == "true" ]
then
    # Enable all counters in IA32_PERF_GLOBAL_CTRL (0x38f)
    #   bits 34:32 enabled the three fixed function counters
    #	bits 3:0 enable the 4 programmable counters for the logical processor
    echo "Checking IA32_PERF_GLOBAL_CTRL on all cores"
    echo "  (should be 000000070000000f)"
    for core in `seq 0 $MAXCORE`
    do
        echo -n "$core "

        #   Fixed Function 1, 2,3                                       4 IA_32_PMCx enable
        #	         0111                0000 0000 0000 0000 0000 0000 0000        1111
        # 0x0000000   7                   0    0    0    0    0    0    0           f
        $MSR_TOOS_DIR/wrmsr -p $core 0x38f 0x000000070000000f
        $MSR_TOOS_DIR/rdmsr -p $core -x -0 0x38f

    done
fi


# Logical Core Performance Counter Event Select MSRs
#   Counter	 MSR
#	   0    0x186
#	   1    0x187
#	   2    0x188
#	   3    0x189

# Dump all performance counter event select registers on all cores
if [ "$PRINT_PERFEVTSEL" == "true" ]
then
	echo "Printing out all performance counter event select registers"
	echo "MSR    CORE    CurrentValue"
	for PMC_MSR in 186 187 188 189
	do
		for CORE in `seq 0 $MAXCORE`
		do
			echo -n "$PMC_MSR $CORE "
			$MSR_TOOS_DIR/rdmsr -p $core -0 -x 0x${PMC_MSR}
		 done
	done
fi

# Dump all performance counter event select registers on all cores
if [ "$ACTIVATE_PMC" == "true" ]
then
    #If ($core) and ($core + 1) represents the two logical threads of the same physical core
    for core in `seq 0 $MAXCORE`
    do
            echo "ACTIVATING PMC  $core"
#        if [ $(($core % 2 )) -eq 0 ] ; then
            $MSR_TOOS_DIR/wrmsr -p $core 0x186 0x004301a1 # Counter 0 Uops Dispatched on Port 0
            $MSR_TOOS_DIR/wrmsr -p $core 0x187 0x004302a1 # Counter 1 Uops Dispatched on Port 1
            $MSR_TOOS_DIR/wrmsr -p $core 0x188 0x004304a1 # Counter 2 Uops Dispatched on Port 2
            $MSR_TOOS_DIR/wrmsr -p $core 0x189 0x004308a1 # Counter 3 Uops Dispatched on Port 3
#        else
#            $MSR_TOOS_DIR/wrmsr -p $core 0x186 0x0043003c # Counter 4 actual core cycles unhalted
#            $MSR_TOOS_DIR/wrmsr -p $core 0x187 0x004320a1 # Counter 5 Uops Dispatched on Port 5
#            $MSR_TOOS_DIR/wrmsr -p $core 0x188 0x0143019c # Counter 6 cycles with no uops delivered
#            $MSR_TOOS_DIR/wrmsr -p $core 0x189 0x0043010e # Counter 7 Uops issued from RAT to RS
#        fi
    done
else
    #If ($core) and ($core + 1) represents the two logical threads of the same physical core
    for core in `seq 0 $MAXCORE`
    do
            echo "DESACTIVATING PMC  $core"
#        if [ $(($core % 2 )) -eq 0 ] ; then
            $MSR_TOOS_DIR/wrmsr -p $core 0x186 0
            $MSR_TOOS_DIR/wrmsr -p $core 0x187 0
            $MSR_TOOS_DIR/wrmsr -p $core 0x188 0
            $MSR_TOOS_DIR/wrmsr -p $core 0x189 0
#        else
#            $MSR_TOOS_DIR/wrmsr -p $core 0
#            $MSR_TOOS_DIR/wrmsr -p $core 0
#            $MSR_TOOS_DIR/wrmsr -p $core 0
#            $MSR_TOOS_DIR/wrmsr -p $core 0
#        fi
    done
fi




if [ "$RESET_PMC" == "true" ]
then
    for core in `seq 0 $MAXCORE`
    do
        echo "RESET PMC $core"
        $MSR_TOOS_DIR/wrmsr -p $core 0x4C1 0
        $MSR_TOOS_DIR/wrmsr -p $core 0x4C2 0
        $MSR_TOOS_DIR/wrmsr -p $core 0x4C3 0
        $MSR_TOOS_DIR/wrmsr -p $core 0x4C4 0
    done
fi


if [ "$ACTIVATE_FIXED_FUNCTION" == "true" ]
then
    for core in `seq 0 $MAXCORE`
    do
        echo "ACTIVATING FIXED FUNCTION CORE $core"
        $MSR_TOOS_DIR/wrmsr -p $core 0x38D 0x333
    done
else
    for core in `seq 0 $MAXCORE`
    do
        echo "DESACTIVATING FIXED FUNCTION CORE $core"
        $MSR_TOOS_DIR/wrmsr -p $core 0x38D 0
    done
fi




if [ "$RESET_FIXED_FUNCTION" == "true" ]
then
    echo "RESET FIXED FUNCTION"
    for core in `seq 0 $MAXCORE`
    do
        $MSR_TOOS_DIR/wrmsr -p $core 0x309 0
        $MSR_TOOS_DIR/wrmsr -p $core 0x30A 0
        $MSR_TOOS_DIR/wrmsr -p $core 0x30B 0
    done
fi



if [ "$READ_FIXED_FUNCTION" == "true" ]
then

    for core in `seq 0 $MAXCORE`
    do
        echo "READ FIXED FUNCTION CORE $core"
        printf "Fixed 1 - " ; $MSR_TOOS_DIR/rdmsr -p $core -0 -x 0x309
        printf "Fixed 2 - " ; $MSR_TOOS_DIR/rdmsr -p $core -0 -x 0x30A
        printf "Fixed 3 - " ; $MSR_TOOS_DIR/rdmsr -p $core -0 -x 0x30B
    done
fi