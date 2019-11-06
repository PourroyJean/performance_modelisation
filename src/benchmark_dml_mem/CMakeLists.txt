
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.cxx")
file(GLOB COMMON_SOURCES "../common" "*.cxx")


if (UNIX)
    #do something
endif (UNIX)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # using Clang
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -march=skylake-avx512")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -march=skylake-avx512 -qopt-streaming-stores=always")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # using Visual Studio C++
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")
endif()

option(OPT_BUILD_MPI "Build the benchmark with MPI" OFF)

set (DML_EXECUTABLE "xxx")


#REQUIRED OR NOT
if(OPT_BUILD_MPI)
    find_package(MPI REQUIRED)

    if (MPI_FOUND)
        include_directories(${MPI_INCLUDE_PATH})
        message("-- The benchmark will be compiled with MPI library" )
        add_definitions(-DCOMPILED_WITH_MPI)
        set (DML_EXECUTABLE "dml_mpi")
    else ()
        #should never happen as MPI is required
        message(WARNING "MPI requested but not found by Cmake")
        set (DML_EXECUTABLE "dml")

    endif()
else()
    message("MPI NOT REQUIRED")
    set (DML_EXECUTABLE "dml")
endif()





#Generate the code annotation library
add_library(libAnno SHARED IMPORTED)
set_property(TARGET libAnno PROPERTY IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/lib/libcodeAnnotation.so")



include_directories(../common)
include_directories(../tool_yamb/)


add_executable(${DML_EXECUTABLE} dml_main.cpp dml_parameters.cpp dml_benchmark.cpp ${COMMON_SOURCES} dml_misc.cpp)


#INCLUDE DIRECTORY
target_include_directories(${DML_EXECUTABLE} PRIVATE ${MPI_CXX_INCLUDE_PATH})

#LINK LIBRARIES
target_link_libraries (${DML_EXECUTABLE} "misc" )
target_link_libraries (${DML_EXECUTABLE} "${CMAKE_BINARY_DIR}/lib/libcodeAnnotation.so"  )
target_link_libraries (${DML_EXECUTABLE} "${MPI_CXX_LIBRARIES}")



if(MPI_COMPILE_FLAGS)
    set_target_properties(${DML_EXECUTABLE} PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
    set_target_properties(${DML_EXECUTABLE} PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()


set_target_properties( ${DML_EXECUTABLE}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/benchmark_dml_mem"
)


#MESSAGE("MPI_CXX_LIBRARIES = ${MPI_CXX_LIBRARIES}")


#file(GLOB KG_COPY_FILES
#        "script/*.py"
#        "script/*.sh"
#        "kg_template_start.cpp"
#        "kg_template_end.cpp"
#        )
#
#
#file(COPY ${KG_COPY_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin/benchmark_kernel_generator)






