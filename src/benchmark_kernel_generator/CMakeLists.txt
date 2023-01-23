
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.cxx")
#message(WARNING "-- YOLO    --" ${CMAKE_SOURCE_DIR})
#message(WARNING "-- YOLO    --" ${CMAKE_CURRENT_LIST_FILE})

set(CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")




include_directories(../common)


add_library(lib_kg kg_parameters.cpp kg_misc.cpp kg_generators.cpp kg_executor.cpp)
add_executable(kg kg_main.cxx kg_parameters.cpp kg_misc.cpp kg_generators.cpp kg_executor.cpp)

target_link_libraries(kg misc)


set_target_properties( kg
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/benchmark_kernel_generator"
)


file(GLOB KG_COPY_FILES
        "../../script/benchmark_kernel_generator/script/*.py"
        "../../script/benchmark_kernel_generator/script/*.sh"
        "kg.template.cpp"
        )


file(COPY ${KG_COPY_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin/benchmark_kernel_generator)






