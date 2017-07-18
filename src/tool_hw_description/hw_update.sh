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
echo "#!/usr/bin/bash" > $FILE


echo "############################### " >> $FILE
echo "#########  GENERAL ############ " >> $FILE
echo "############################### " >> $FILE

echo "#-- CPU GENERATION --" >> $FILE
echo "export PM_HW_CPU_GENERATION=\"$PM_HW_CPU_GENERATION\"" >> $FILE
echo "" >> $FILE

echo "#-- CPU NAME --" >> $FILE
echo "export PM_HW_CPU_NAME=\"$PM_HW_CPU_NAME\"" >> $FILE
echo "" >> $FILE

echo "#-- Number of sockets on the server --" >> $FILE
echo "export PM_HW_SOCKET=$PM_HW_SOCKET" >> $FILE
echo "" >> $FILE

echo "#-- Number of physical cores --" >> $FILE
echo "export PM_HW_CORE_PHY=$PM_HW_CPU_CORE_PHY" >> $FILE
echo "" >> $FILE

echo "#-- Number of threads --" >> $FILE
echo "export PM_HW_THREADS=$PM_HW_CPU_THREADS" >> $FILE
echo "" >> $FILE

echo "#-- Frequency base --" >> $FILE
echo "export PM_HW_CPU_FREQ_BASE=$PM_HW_CPU_FREQ_BASE" >> $FILE
echo "" >> $FILE

echo "#-- Has Turbo ON ? --" >> $FILE
echo "export PM_HW_CPU_FREQ_TURBO=$PM_HW_CPU_HAS_TURBO" >> $FILE
echo "" >> $FILE



echo "\n\n" >> $FILE
echo "############################### " >> $FILE
echo "#########  MEMORY  ############ " >> $FILE
echo "############################### " >> $FILE


echo "#-- CACHE L1 --" >> $FILE
echo "" >> $FILE

echo "#-- CACHE L2 --" >> $FILE
echo "" >> $FILE

echo "#-- CACHE L3 --" >> $FILE
echo "" >> $FILE

echo "#-- LAST LEVEL CACHE (L4, MCDRAM...) --" >> $FILE
echo "" >> $FILE



