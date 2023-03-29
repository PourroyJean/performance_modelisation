#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.c")

option(OPT_KG "Include Kernel Generator sub-project" OFF)
option(OPT_DML "Include DML sub-project" OFF)

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



# << Choose the required tool by commenting or not >>
if(OPT_KG)
    add_subdirectory(benchmark_kernel_generator)
endif()

if(OPT_DML)
    add_subdirectory(benchmark_dml_mem)
endif()

#add_subdirectory (benchmark_multiply)
#add_subdirectory (tool_yamb)
#add_subdirectory(tool_oprofile)
