
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.cxx")
#message(WARNING "-- YOLO    --" ${CMAKE_SOURCE_DIR})
#message(WARNING "-- YOLO    --" ${CMAKE_CURRENT_LIST_FILE})

set(CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")




include_directories(../common)


#add_library(lib_kg kg_parameters.cpp kg_misc.cpp kg_generators.cpp kg_executor.cpp)
add_executable(mmb monitormembw.c)

#target_link_libraries(kg misc)


set_target_properties(mmb
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tool_monitor_memory_bandwidth"
)


file(GLOB MMB_COPY_FILES
#        "script/*.py"
        "script/*.sh"
#        "kg_template_start.cpp"
        )


file(COPY ${MMB_COPY_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin/tool_monitor_memory_bandwidth/)






