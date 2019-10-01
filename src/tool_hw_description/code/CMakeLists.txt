
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.cxx")


set(CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")



#add_subdirectory ("../../benchmark_kernel_generator")

#include_directories(${CMAKE_SOURCE_DIR}/src/benchmark_kernel_generator)



#add_library(lib_kg kg_parameters.cpp kg_misc.cpp kg_generators.cpp kg_executor.cpp)
#add_executable(benchmark_kernel_generator benchmark_kernel_generator.cxx kg_parameters.cpp kg_misc.cpp kg_generators.cpp kg_executor.cpp)

add_executable(get_hw_has_turbo get_hw_has_turbo.cxx)

set_target_properties( get_hw_has_turbo
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tool_hw_description/getter"
)


#file(GLOB KG_COPY_FILES
#        "${CMAKE_SOURCE_DIR}/src/benchmark_kernel_generator/script/*.py"
#        "${CMAKE_SOURCE_DIR}/src/benchmark_kernel_generator/script/*.sh"
#        )
#file(COPY ${KG_COPY_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin/benchmark_kernel_generator)