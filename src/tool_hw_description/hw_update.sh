#!/usr/bin/env bash



PATH_TOOL="${BASH_SOURCE[0]}";
if ([ -h "${PATH_TOOL}" ]) then
  while([ -h "${PATH_TOOL}" ]) do PATH_TOOL=`readlink "${PATH_TOOL}"`; done
fi
pushd . > /dev/null
cd `dirname ${PATH_TOOL}` > /dev/null
PATH_TOOL=`pwd`;
popd  > /dev/null


PATH_SCRIPT=$PATH_TOOL/script

FILE=$PATH_TOOL/"hw_data.sh"
rm $FILE
touch $FILE

echo "#########  GENERAL ############ " >> $FILE

echo "#-- CPU NAME --" >> $FILE
echo "export PM_HW_CPU_NAME=\"$PM_HW_CPU_NAME\"" >> $FILE
echo "" >> $FILE
echo "#-- Number of sockets on the server --" >> $FILE
echo "export PM_HW_SOCKET=$PM_HW_SOCKET" >> $FILE
echo "" >> $FILE
echo "#-- Number of physical cores --" >> $FILE
echo "export PM_HW_CORE_PHY=$PM_HW_CORE_PHY" >> $FILE
echo "" >> $FILE
echo "#-- Number of threads --" >> $FILE
echo "export PM_HW_THREADS=$PM_HW_THREADS" >> $FILE
echo "" >> $FILE
echo "#-- Frequency base --" >> $FILE
echo "export PM_HW_CPU_FREQ_BASE=$PM_HW_CPU_FREQ_BASE" >> $FILE
echo "" >> $FILE
echo "#-- Has Turbo ON ? --" >> $FILE
echo "export PM_HW_CPU_FREQ_TURBO=$PM_HW_CPU_HAS_TURBO" >> $FILE




