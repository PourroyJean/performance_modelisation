
#However, the file(GLOB...) allows for wildcard additions:
#file(GLOB SOURCES "*.cxx")
#message(WARNING "-- YOLO    --" ${CMAKE_SOURCE_DIR})
#message(WARNING "-- YOLO    --" ${CMAKE_CURRENT_LIST_FILE})

set(CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")




#include_directories(../common)


#add_library(lib_kg kg_parameters.cpp kg_misc.cpp kg_generators.cpp kg_executor.cpp)
#add_executable(kg kg_main.cxx kg_parameters.cpp kg_misc.cpp kg_generators.cpp kg_executor.cpp)
#
#target_link_libraries(kg misc)
#
#
#set_target_properties( kg
#        PROPERTIES
#        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tool_kernel_generator"
#)
#


set(INSTALL_PATH "${CMAKE_BINARY_DIR}/bin/tool_monitoring_bandwidth/")



# ------- LIBRARY CODE ANNOTATION ----------

include_directories(include)
file(GLOB SOURCES "code_annotation.cpp")

add_library(codeAnnotation SHARED ${SOURCES})
install(TARGETS codeAnnotation DESTINATION ${CMAKE_BINARY_DIR}/bin/tool_monitoring_bandwidth/lib)


# Install library headers
file(GLOB HEADERS code_annotation.h)
install(FILES ${HEADERS} DESTINATION ${CMAKE_BINARY_DIR}/bin/tool_monitoring_bandwidth/include)












#
#set(INSTALL_PATH "${CMAKE_BINARY_DIR}/bin/tool_monitoring_bandwidth/")
#
#message(WARNING  ${INSTALL_PATH})
#
## Find source files
#file(GLOB SOURCES foo.cpp)
#
## Include header files
#include_directories(.)
#
## Create shared library
#add_library(codeAnnotationZZ SHARED ${SOURCES})
#
## Install library
#install(TARGETS codeAnnotationZZ DESTINATION ${INSTALL_PATH}/lib/)
#
## Install library headers
#file(GLOB HEADERS code_annotation.h)
#install(FILES ${HEADERS} DESTINATION /nfs/pourroy/code/THESE/performance_modelisation/build/bin/tool_monitoring_bandwidth/include)
#
#
#






#add_library(yambLibCodeAnnotation SHARED
#        code_annotation.cpp)

#set_target_properties(yambLibCodeAnnotation PROPERTIES VERSION ${PROJECT_VERSION})
#set_target_properties(yambLibCodeAnnotation PROPERTIES PUBLIC_HEADER code_annotation.h)
#
#install(TARGETS yambLibCodeAnnotation
#        LIBRARY DESTINATION LIB ${CMAKE_BINARY_DIR}/bin/tool_monitoring_bandwidth/lib #${CMAKE_INSTALL_LIBDIR}
#        PUBLIC_HEADER DESTINATION ${CMAKE_BINARY_DIR}/bin/tool_monitoring_bandwidth/include)
#
#
#





#file(GLOB BW_COPY_FILES
#        "monitoring_bw_main.sh"
#        "format_log.py"
#        )



#
#
#file(COPY ${BW_COPY_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin/tool_monitoring_bandwidth)
#
#
#
#file(GLOB HEADER_COPY_FILES
#        "code_annotation.h"
#        )
#file(COPY ${HEADER_COPY_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin/tool_monitoring_bandwidth/include)
#




