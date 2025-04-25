cmake_minimum_required(VERSION 3.15)

project(DmlBenchmark CXX)

option(DML_BUILD_MPI "Build with MPI support" OFF)
option(DML_USE_YAMB "Enable YAMB library annotation" OFF)

set(DML_INST_TYPE "native" CACHE STRING "Target CPU microarchitecture for optimization")
message(STATUS "+++ Target Architecture Optimization (-march): ${DML_INST_TYPE}")

get_filename_component(CMAKE_CURRENT_LIST_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

# Define source files for the executable
set(DML_EXECUTABLE_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/dml_main.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/dml_parameters.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/dml_benchmark.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/dml_misc.cpp"
)

set(CXX_STANDARD_FLAG "-std=c++11")
set(ARCH_OPT_FLAG "-march=${DML_INST_TYPE}")
set(BASE_OPT_FLAGS "-O3")
set(LTO_FLAG "-flto")

set(DML_EXECUTABLE_NAME "dml")
set(MPI_DEFINITIONS "")
set(MPI_INCLUDE_DIRS "")
set(MPI_LIBRARIES "")
set(MPI_COMPILE_FLAGS "")
set(MPI_LINK_FLAGS "")

# Find OpenMP
find_package(OpenMP REQUIRED)

# Handle MPI build options
if(DML_BUILD_MPI)
    find_package(MPI REQUIRED)
    if (MPI_FOUND)
        message(STATUS "+++ MPI Found: Building with MPI support.")
        set(DML_EXECUTABLE_NAME "dml_mpi")
        set(MPI_DEFINITIONS "-DCOMPILED_WITH_MPI")
        set(MPI_INCLUDE_DIRS ${MPI_CXX_INCLUDE_PATH})
        set(MPI_LIBRARIES ${MPI_CXX_LIBRARIES})
        set(MPI_COMPILE_FLAGS ${MPI_CXX_COMPILE_FLAGS})
        set(MPI_LINK_FLAGS ${MPI_CXX_LINK_FLAGS})
    else()
        message(WARNING "/!\ MPI requested but not found by CMake.")
    endif()
else()
    message(STATUS "+++ MPI support disabled.")
endif()

set(YAMB_DEFINITIONS "")
set(YAMB_LIBRARIES "")
# Handle YAMB annotation options
if(DML_USE_YAMB)
    message(STATUS "+++ YAMB annotation enabled.")
    set(YAMB_LIB_PATH "${CMAKE_BINARY_DIR}/lib/libcodeAnnotation.so")
    if(EXISTS "${YAMB_LIB_PATH}")
         add_library(libAnno SHARED IMPORTED GLOBAL)
         set_property(TARGET libAnno PROPERTY IMPORTED_LOCATION "${YAMB_LIB_PATH}")
         set(YAMB_LIBRARIES libAnno)
         set(YAMB_DEFINITIONS "-DYAMB_ENABLED")
    else()
        message(WARNING "/!\ YAMB enabled, but library not found at ${YAMB_LIB_PATH}. Annotation will not be linked.")
    endif()
else()
    message(STATUS "+++ YAMB annotation disabled.")
endif()

add_executable(${DML_EXECUTABLE_NAME} ${DML_EXECUTABLE_SOURCES})

get_filename_component(CMAKE_CURRENT_LIST_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

target_include_directories(${DML_EXECUTABLE_NAME} PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/../tool_yamb"
    ${MPI_INCLUDE_DIRS}
)

target_compile_definitions(${DML_EXECUTABLE_NAME} PRIVATE
    ${MPI_DEFINITIONS}
    ${YAMB_DEFINITIONS}
)

target_compile_options(${DML_EXECUTABLE_NAME} PRIVATE
    ${CXX_STANDARD_FLAG}
    ${BASE_OPT_FLAGS}
    ${ARCH_OPT_FLAG}
    ${MPI_COMPILE_FLAGS}
    ${OpenMP_CXX_FLAGS}
)

# Compiler-specific flags
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang|GNU")
    target_compile_options(${DML_EXECUTABLE_NAME} PRIVATE
        "-fopenmp-simd"
        "-funroll-loops"
        "-ffast-math"
        "-fno-exceptions"
        ${LTO_FLAG}
    )
    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
         if("${DML_INST_TYPE}" MATCHES "znver[0-9]+")
             target_compile_options(${DML_EXECUTABLE_NAME} PRIVATE 
             "-fnt-store"
             "-Rpass=loop-vectorize" 
             "-Rpass-missed=loop-vectorize" 
             "-Rpass-analysis=loop-vectorize"
             )
             message(STATUS "+++ Added Clang/AOCC AMD-specific flag: -fnt-store")
         endif()
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_compile_options(${DML_EXECUTABLE_NAME} PRIVATE
            "-fprefetch-loop-arrays"
            "-fomit-frame-pointer"
            "-fstrict-aliasing"
        )
        message(STATUS "+++ Added GCC-specific flags: -fprefetch-loop-arrays -fomit-frame-pointer")
    endif()

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "IntelLLVM")
    target_compile_options(${DML_EXECUTABLE_NAME} PRIVATE
        "-ftz"
        "-qopenmp-simd"
    )

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options(${DML_EXECUTABLE_NAME} PRIVATE
        "/O2"
        "/arch:AVX2"
        "/EHsc-"
        "/openmp:experimental"
    )
    set(LTO_FLAG "/GL")
    message(STATUS "+++ Added MSVC-specific flags")

else()
    message(WARNING "+++ Unknown CXX Compiler ID: ${CMAKE_CXX_COMPILER_ID}. Applying base optimizations only.")
endif()

target_link_libraries(${DML_EXECUTABLE_NAME} PRIVATE
    misc
    anyOptionlib
    ${MPI_LIBRARIES}
    ${YAMB_LIBRARIES}
    ${OpenMP_CXX_LIBRARIES}
)

target_link_options(${DML_EXECUTABLE_NAME} PRIVATE
    ${LTO_FLAG}
    ${OpenMP_CXX_FLAGS}
)

# Set output directory for the executable
set_target_properties(${DML_EXECUTABLE_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/benchmark_dml_mem"
)

message(STATUS "+++ Executable: ${DML_EXECUTABLE_NAME}")
message(STATUS "+++ Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "+++ CXX Compiler: ${CMAKE_CXX_COMPILER} (ID: ${CMAKE_CXX_COMPILER_ID})")
message(STATUS "+++ CXX Flags: ${CMAKE_CXX_FLAGS} (Note: Target-specific flags applied separately)")

get_target_property(TARGET_COMPILE_OPTIONS ${DML_EXECUTABLE_NAME} COMPILE_OPTIONS)
message(STATUS "+++ Final compile options for ${DML_EXECUTABLE_NAME}: ${TARGET_COMPILE_OPTIONS}")

get_target_property(TARGET_LINK_OPTIONS ${DML_EXECUTABLE_NAME} LINK_OPTIONS)
message(STATUS "+++ Final link options for ${DML_EXECUTABLE_NAME}: ${TARGET_LINK_OPTIONS}")