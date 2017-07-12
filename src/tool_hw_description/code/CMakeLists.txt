
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.cxx")


set(CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")



add_subdirectory ("tool_freq")

#include_directories(${CMAKE_SOURCE_DIR}/src/tool_freq)



#add_library(lib_tool_freq tool_freq_parameters.cpp tool_freq_misc.cpp tool_freq_generators.cpp tool_freq_executor.cpp)
#add_executable(tool_freq tool_freq.cxx tool_freq_parameters.cpp tool_freq_misc.cpp tool_freq_generators.cpp tool_freq_executor.cpp)

add_executable(get_hw_has_turbo get_hw_has_turbo.cxx)

set_target_properties( get_hw_has_turbo
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tool_hw_description/getter"
)


#file(GLOB TOOL_FREQ_COPY_FILES
#        "${CMAKE_SOURCE_DIR}/src/tool_freq/script/*.py"
#        "${CMAKE_SOURCE_DIR}/src/tool_freq/script/*.sh"
#        )
#file(COPY ${TOOL_FREQ_COPY_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin/tool_freq)