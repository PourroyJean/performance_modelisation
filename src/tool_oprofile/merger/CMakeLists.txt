#message(WARNING "-- YOLO    --" ${CMAKE_CURRENT_LIST_FILE})

set(CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")



#Main executable
add_executable(merger op_merger_main.cxx op_misc.cpp op_objdump_line.cxx op_oprofile_line.cxx op_misc.cpp Line.cpp Line_Objdump.cpp Line_Oprofile.cpp InputFile.cpp)


#We use the common/misc codde
include_directories(../common)
target_link_libraries(merger misc)


#Move needed files
set_target_properties(merger
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tool_oprofile/"
)



#Used to PRINT DEBUG
add_definitions(-DOP_DEBUG)






