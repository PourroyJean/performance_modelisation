
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.cxx")


set(CMAKE_C_FLAGS "-g --std=gnu99 ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

#add_executable(benchmark_multiply ${SOURCES})
add_executable(benchmark_multiply benchmark_multiply.cpp multiply_version.cpp multiply_version.h)

set_target_properties( benchmark_multiply
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/benchmark_multiply/"
)


file(GLOB KG_COPY_FILES
        "${CMAKE_SOURCE_DIR}/src/benchmark_multiply/*.sh"
        )
file(COPY ${KG_COPY_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin/benchmark_multiply)
