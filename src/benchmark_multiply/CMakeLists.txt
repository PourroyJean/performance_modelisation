
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.cxx")


set(CMAKE_C_FLAGS "-g --std=gnu99 ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(../common)
file(GLOB COMMON_SOURCES "../common" "*.cxx")



if(DEFINED BM_OMP AND  BM_OMP STREQUAL "TRUE")
    message("-- BM -- OPENMP ON")
    find_package(OpenMP)
    if (OPENMP_FOUND)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBM_OMP")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

        if (BM_OMP_TARGET_GPU)
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBM_OMP_TARGET_GPU")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp -fopenmp-targets=amdgcn-amd-amdhsa -Xopenmp-target=amdgcn-amd-amdhsa -march=gfx908 ")
        endif ()
    else()
        message("-- BM --  /!\ OPENMP asked but not found /!\ ")
    endif ()
else()
    message("-- BM -- OPENMP OFF")
endif()




add_executable(benchmark_multiply bm_main.cpp bm_bench_version.cpp ${COMMON_SOURCES})
target_link_libraries(benchmark_multiply "misc")


set_target_properties(benchmark_multiply
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/benchmark_multiply/"
        )


file(GLOB KG_COPY_FILES
        "${CMAKE_SOURCE_DIR}/src/benchmark_multiply/*.sh"
        )
file(COPY ${KG_COPY_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin/benchmark_multiply)
