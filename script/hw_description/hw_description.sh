#!/usr/bin/env bash

#Call to get_hw_threads.sh
export PM_HW_THREADS=$(get_hw_threads.sh)

#Call to get_hw_core_phy.sh
export PM_HW_CORE_PHY=$(get_hw_core_phy.sh)

#Call to get_hw_cpu_freq_base.sh
export PM_HW_CPU_FREQ_BASE=$(get_hw_freq_base.sh)

#Call to get_hw_cpu_has_turbo.sh
export PM_HW_CPU_HAS_TURBO=$(get_hw_has_turbo.sh)

#Call to get_hw_cpu_name.sh
export PM_HW_CPU_NAME=$(hw_cpu_name.sh)

#Call to get_hw_socket.sh
export PM_HW_SOCKET=$(hw_socket.sh)






#Updata the data file: hw_data.sh
hw_update.sh





