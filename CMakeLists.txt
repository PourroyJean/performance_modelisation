cmake_minimum_required(VERSION 2.8.9)
project(performance_modelisation)


#Output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH         ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/")

#log
#message(WARNING "-- source directory --" ${performance_modelisation_SOURCE_DIR})
#message(WARNING "-- bin directory    --" ${performance_modelisation_BINARY_DIR})

#Bring the headers, such as Student.h into the project
include_directories("${CMAKE_SOURCE_DIR}/include")

add_subdirectory (src)
#add_subdirectory (test)

enable_testing()


#find_package(Doxygen REQUIRED)
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
            ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)


#
#file(GLOB S_FILES
#        "${CMAKE_SOURCE_DIR}/script/*"
#        )
#
#file(COPY ${S_FILES}   DESTINATION ${CMAKE_BINARY_DIR}/bin/script)