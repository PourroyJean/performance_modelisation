#!/usr/bin/bash



#Get Script source: https://stackoverflow.com/questions/59895/getting-the-source-directory-of-a-bash-script-from-within
#


PATH_TOOL="${BASH_SOURCE[0]}";
if ([ -h "${PATH_TOOL}" ]) then
  while([ -h "${PATH_TOOL}" ]) do PATH_TOOL=`readlink "${PATH_TOOL}"`; done
fi
pushd . > /dev/null
cd `dirname ${PATH_TOOL}` > /dev/null
PATH_TOOL=`pwd`;
popd  > /dev/null


PATH_SCRIPT=$PATH_TOOL/script



echo "Current execution: $PATH_TOOL"
echo "  - Your path:             $(pwd)"
echo "  - hw_description path:   $PATH_TOOL"
echo "  - hw_description script: $PATH_SCRIPT"




#Call to get_hw_cpu_name.sh
export PM_HW_CPU_NAME=$($PATH_SCRIPT/get_hw_cpu_name.sh)

#Call to get_hw_socket.sh
export PM_HW_SOCKET=$($PATH_SCRIPT/get_hw_socket.sh)

#Call to get_hw_core_phy.sh
export PM_HW_CORE_PHY=$($PATH_SCRIPT/get_hw_core_phy.sh)

#Call to get_hw_threads.sh
export PM_HW_THREADS=$($PATH_SCRIPT/get_hw_threads.sh)

#Call to get_hw_cpu_freq_base.sh
export PM_HW_CPU_FREQ_BASE=$($PATH_SCRIPT/get_hw_cpu_freq_base.sh)

#Call to get_hw_cpu_has_turbo.sh
export PM_HW_CPU_HAS_TURBO=$($PATH_SCRIPT/get_hw_cpu_has_turbo.sh)




#Updata the data file: hw_data.sh
source $PATH_TOOL/hw_update.sh





