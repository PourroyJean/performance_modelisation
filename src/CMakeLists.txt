
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.c")

add_executable(main_programme ${SOURCES})
add_library(ma_lib Student.c)
