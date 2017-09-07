
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.cxx")


set(CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")



#add_executable(benchmark_multiply ${SOURCES})
add_executable(benchmark_multiply benchmark_multiply.cpp)

set_target_properties( benchmark_multiply
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/benchmark_multiply/"
)


#file(GLOB KG_COPY_FILES
#        "${CMAKE_SOURCE_DIR}/src/tool_kernel_generator/script/*.py"
#        "${CMAKE_SOURCE_DIR}/src/tool_kernel_generator/script/*.sh"
#        )
#file(COPY ${KG_COPY_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin/tool_kernel_generator)