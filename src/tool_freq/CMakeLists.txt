
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
        )

add_library(lib_tool_freq tool_freq_misc.cpp)


#add_executable(tool_freq tool_freq.c misc.c)
add_executable(tool_freq tool_freq.cxx tool_freq_misc.cpp)
add_executable(tool_freq_generator assembly_generator.cpp)

add_dependencies(tool_freq CopyScript)
