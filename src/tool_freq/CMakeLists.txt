
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.cxx")
#message(WARNING "-- YOLO    --" ${CMAKE_SOURCE_DIR})

set(CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")



include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src/tool_freq)


#copy the generator script
add_custom_target(CopyScript
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/script/tool_freq_compile.sh ${CMAKE_BINARY_DIR}/bin/tool_freq_compile.sh
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/script/normal_distribution.py ${CMAKE_BINARY_DIR}/bin/normal_distribution.py
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/script/tool_freq_graphical.sh ${CMAKE_BINARY_DIR}/bin/tool_freq_graphical.sh
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/tool_freq/tool_freq_template_start.cpp ${CMAKE_BINARY_DIR}/bin/tool_freq_template_start.cpp
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/tool_freq/tool_freq_template_end.cpp ${CMAKE_BINARY_DIR}/bin/tool_freq_template_end.cpp
        )

add_library(lib_tool_freq tool_freq_parameters.cpp tool_freq_misc.cpp tool_freq_generators.cpp tool_freq_executor.cpp)


#add_executable(tool_freq tool_freq.c misc.c)
add_executable(tool_freq tool_freq.cxx tool_freq_parameters.cpp tool_freq_misc.cpp tool_freq_generators.cpp tool_freq_executor.cpp)

add_dependencies(tool_freq CopyScript )
