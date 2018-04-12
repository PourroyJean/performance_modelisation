
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.cxx")
#message(WARNING "-- YOLO    --" ${CMAKE_SOURCE_DIR})
#message(WARNING "-- YOLO    --" ${CMAKE_CURRENT_LIST_FILE})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")




include_directories(../common)


#add_library(lib_bm bm_parameters.cpp)

add_executable(bm bm_main.cpp bm_parameters.cpp )

#target_link_libraries(kg misc)


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






