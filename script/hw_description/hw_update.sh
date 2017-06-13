#!/usr/bin/env bash


FILE="hw_data.sh"


rm $FILE

echo $PM_HW_CORE


echo "#########  GENERAL ############ " >> $FILE
echo "export PM_HW_CPU_NAME=$PM_HW_CPU_NAME" >> $FILE
echo "export PM_HW_SOCKET=$PM_HW_SOCKET" >> $FILE
echo "export PM_HW_CORE_PHY=$PM_HW_CORE_PHY" >> $FILE
echo "export PM_HW_THREADS=$PM_HW_CORE_THREADS" >> $FILE
echo "export PM_HW_CPU_FREQ_BASE=$PM_HW_CPU_FREQ_BASE" >> $FILE
echo "export PM_HW_CPU_FREQ_TURBO=$PM_HW_CPU_FREQ_TURBO" >> $FILE
echo "export PM_HW_CPU_NUMA_DOMAIN=$PM_HW_CPU_NUMA_DOMAIN" >> $FILE



