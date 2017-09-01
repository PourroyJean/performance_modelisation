#message(WARNING "-- YOLO    --" ${CMAKE_SOURCE_DIR})
#message(WARNING "-- YOLO    --" ${CMAKE_CURRENT_LIST_FILE})

add_subdirectory (merger)



#
file(GLOB KG_COPY_FILES
        "op_analysis.sh"
        "op_stop.sh"
        "op_start.sh"
        "op_main.sh"
)


file(COPY ${KG_COPY_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin/tool_oprofile)






