
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.cxx")
#message(WARNING "-- YOLO    --" ${CMAKE_SOURCE_DIR})
#message(WARNING "-- YOLO    --" ${CMAKE_CURRENT_LIST_FILE})

set(CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")



include_directories(${CMAKE_SOURCE_DIR}/include)
#include_directories(${CMAKE_SOURCE_DIR}/src/tool_kernel_generator)



add_library(lib_kg kg_parameters.cpp kg_misc.cpp kg_generators.cpp kg_executor.cpp)
add_executable(kg kg_main.cxx kg_parameters.cpp kg_misc.cpp kg_generators.cpp kg_executor.cpp)


set_target_properties( kg
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tool_hw_description/kg"
)


file(GLOB KG_COPY_FILES
        "script/*.py"
        "script/*.sh"
        "kg_template_start.cpp"
        "kg_template_end.cpp"
        )


file(COPY ${KG_COPY_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin/tool_hw_description/kg)






