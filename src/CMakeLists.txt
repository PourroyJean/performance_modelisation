
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.c")


include_directories(${CMAKE_SOURCE_DIR}/include)
#add_executable(main_programme mainapp.c misc.c)
add_library(lib_misc misc.c)


#add_executable(tool_freq tool_freq.c misc.c)
#add_executable(tool_cacheLineSize tool_cacheLineSize.c misc.c)
#add_executable(tool_cacheSize tool_cacheSize.c misc.c)

add_subdirectory (tool_freq)
add_subdirectory (tool_hw_description)