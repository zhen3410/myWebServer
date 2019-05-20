aux_source_directory(. DIR_LIB_SRCS)

include_directories(${PROJECT_SOURCE_DIR}/base)

add_executable(testCode ${DIR_LIB_SRCS})

target_link_libraries(testCode libserver_base)

add_subdirectory(base)
