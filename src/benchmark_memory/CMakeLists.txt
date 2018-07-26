
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.cxx")
file(GLOB COMMON_SOURCES "../common" "*.cxx")
#message(WARNING "-- YOLO    --" ${CMAKE_SOURCE_DIR})
#message(WARNING "-- YOLO    --" ${CMAKE_CURRENT_LIST_FILE})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 ")


find_package(MPI)

if (MPI_FOUND)
    #    message(WARNING "-- MPI FOUND --" MPI_C_LIBRARIES)
    include_directories(${MPI_INCLUDE_PATH})
    message(WARNING "-- MPI FOUND --" )
#    m_MPI_FLAG="-D
else ()
message(WARNING "-- MPI NOT FOUND --")
endif()


#Generate the code annotation library
add_library(libAnno SHARED IMPORTED)
set_property(TARGET libAnno PROPERTY IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/lib/libcodeAnnotation.so")



include_directories(../common)
include_directories(../tool_monitoring_bandwidth/)


add_executable(bm bm_main.cpp bm_parameters.cpp bm_benchmark.cpp ${COMMON_SOURCES})

target_link_libraries (bm "${CMAKE_BINARY_DIR}/lib/libcodeAnnotation.so" ${MPI_LIBRARIES})
target_link_libraries (bm "misc")


if(MPI_COMPILE_FLAGS)
    set_target_properties(bm PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
    set_target_properties(bm PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

#add_definitions(-DWITH_MPI)

set_target_properties( bm
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/benchmark_memory"
)


#file(GLOB KG_COPY_FILES
#        "script/*.py"
#        "script/*.sh"
#        "kg_template_start.cpp"
#        "kg_template_end.cpp"
#        )
#
#
#file(COPY ${KG_COPY_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin/tool_kernel_generator)






