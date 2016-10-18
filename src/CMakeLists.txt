cmake_minimum_required (VERSION 2.8)
project (socket)

set(ALLOW_DUPLICATE_CUSTOM_TARGETS 1)

add_subdirectory(process)
add_subdirectory(pthread)
add_subdirectory(select)
add_subdirectory(poll)
