
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.cxx")

set(CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")



#include_directories(${CMAKE_SOURCE_DIR}/include)
#include_directories(${CMAKE_SOURCE_DIR}/src/tool_hw_description)



#Copying all script folder
file(GLOB SCRIPT_FILES
        "${CMAKE_SOURCE_DIR}/src/tool_hw_description/script/*.sh"

        )

file(GLOB MAIN_FILES
        "${CMAKE_SOURCE_DIR}/src/tool_hw_description/hw_data.sh"
        "${CMAKE_SOURCE_DIR}/src/tool_hw_description/hw_description.sh"
        "${CMAKE_SOURCE_DIR}/src/tool_hw_description/hw_update.sh"
        )
file(COPY ${SCRIPT_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin/tool_hw_description/script)
file(COPY ${MAIN_FILES}   DESTINATION ${CMAKE_BINARY_DIR}/bin/tool_hw_description/)