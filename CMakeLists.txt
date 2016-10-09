cmake_minimum_required (VERSION 2.8)
project (webbench)
aux_source_directory (. DIR_SRCS)
add_executable (${PROJECT_NAME} ${DIR_SRCS})
