#message(WARNING "-- YOLO    --" ${CMAKE_SOURCE_DIR})
#message(WARNING "-- YOLO    --" ${CMAKE_CURRENT_LIST_FILE})

add_subdirectory (merger)



#
file(GLOB KG_COPY_FILES
        "report_instructions.txt"
        "op_objdump.txt"
)


file(COPY ${KG_COPY_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin/tool_oprofile)






