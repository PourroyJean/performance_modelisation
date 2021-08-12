#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.c")

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_MACOSX_RPATH 1)
endif()
add_subdirectory(common)
add_subdirectory(common/AnyOption)



# << Choose the required tool by commenting or not >>
add_subdirectory (benchmark_kernel_generator)
#add_subdirectory(tool_oprofile)
#add_subdirectory (tool_yamb)
add_subdirectory (benchmark_dml_mem)
#add_subdirectory (tool_hw_description)
