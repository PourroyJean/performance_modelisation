
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.cxx")

set(CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")



#include_directories(${CMAKE_SOURCE_DIR}/include)
#include_directories(${CMAKE_SOURCE_DIR}/src/tool_hw_description)

add_subdirectory (code)





file(GLOB MAIN_FILES
        "${CMAKE_SOURCE_DIR}/src/tool_hw_description/hw_description.sh"
        "${CMAKE_SOURCE_DIR}/src/tool_hw_description/hw_update.sh"
        )
file(COPY ${MAIN_FILES}   DESTINATION ${CMAKE_BINARY_DIR}/bin/tool_hw_description/)

file(GLOB SCRIPT_FILES
        "${CMAKE_SOURCE_DIR}/src/tool_hw_description/script/*"
        )
file(COPY ${SCRIPT_FILES}   DESTINATION ${CMAKE_BINARY_DIR}/bin/tool_hw_description/script/)