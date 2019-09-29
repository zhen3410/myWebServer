aux_source_directory(. LIB_SRC)

add_library(libserver_base ${LIB_SRC})

target_link_libraries(libserver_base pthread rt)

set_target_properties(libserver_base PROPERTIES OUTPUT_NAME "server_base")


