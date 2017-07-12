
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.cxx")
#message(WARNING "-- YOLO    --" ${CMAKE_SOURCE_DIR})
#message(WARNING "-- YOLO    --" ${CMAKE_CURRENT_LIST_FILE})

set(CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")



include_directories(${CMAKE_SOURCE_DIR}/include)
#include_directories(${CMAKE_SOURCE_DIR}/src/tool_freq)



add_library(lib_tool_freq tool_freq_parameters.cpp tool_freq_misc.cpp tool_freq_generators.cpp tool_freq_executor.cpp)
add_executable(tool_freq tool_freq.cxx tool_freq_parameters.cpp tool_freq_misc.cpp tool_freq_generators.cpp tool_freq_executor.cpp)


set_target_properties( tool_freq
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tool_hw_description/tool_freq"
)


file(GLOB TOOL_FREQ_COPY_FILES
        "script/*.py"
        "script/*.sh"
        "tool_freq_template_start.cpp"
        "tool_freq_template_end.cpp"
        )


file(COPY ${TOOL_FREQ_COPY_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin/tool_hw_description/tool_freq)






