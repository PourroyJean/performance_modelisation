
#However, the file(GLOB...) allows for wildcard additions:
#message(WARNING "-- YOLO    --" ${CMAKE_SOURCE_DIR})
#message(WARNING "-- YOLO    --" ${CMAKE_CURRENT_LIST_FILE})

set(CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")



include_directories(${CMAKE_SOURCE_DIR}/include)
#include_directories(${CMAKE_SOURCE_DIR}/src/tool_kernel_generator)



add_library(lib_op op_merger_main.cxx )
add_executable(merger op_merger_main.cxx op_misc.cpp op_objdump_line.cxx op_oprofile_line.cxx)


set_target_properties(merger
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tool_oprofile/"
)





