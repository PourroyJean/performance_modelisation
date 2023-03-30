#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.c")

option(OPT_KG       "Include Kernel Generator sub-project" OFF)
option(OPT_DML      "Include DML sub-project" OFF)
option(OPT_MULTIPLY "Include multiply benchmark sub-project" OFF)
option(OPT_YAMB     "Include yamb tool sub-project" OFF)
option(OPT_OPROFILE "Include oprofile tool sub-project" OFF)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_MACOSX_RPATH 1)
endif()

add_subdirectory(common)
add_subdirectory(common/AnyOption)



if(DEFINED OP_DEBUG AND  OP_DEBUG STREQUAL "TRUE")
    message("+++ DEBUG ON")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOP_DEBUG")
else()
    message("+++ DEBUG OFF")
endif()



if(OPT_KG)
    add_subdirectory(benchmark_kernel_generator)
endif()

if(OPT_DML)
    add_subdirectory(benchmark_dml_mem)
endif()

if(OPT_MULTIPLY)
    add_subdirectory(benchmark_multiply)
endif()

if(OPT_YAMB)
    add_subdirectory(tool_yamb)
endif()

if(OPT_OPROFILE)
    add_subdirectory(tool_oprofile)
endif()
