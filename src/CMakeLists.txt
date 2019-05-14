aux_source_directory(. DIR_LIB_SRCS)

include_directories(${PROJECT_SOURCE_DIR}/base)

add_executable(testRunAfter ${DIR_LIB_SRCS})

target_link_libraries(testRunAfter libserver_base)

add_subdirectory(base)
