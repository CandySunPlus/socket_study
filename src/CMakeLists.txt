cmake_minimum_required (VERSION 2.8)
project (socket)

include(CMakeForceCompiler)
set(CMAKE_C_COMPILER /Users/niksun/Development/sdk/RaspberryPi.platform/Developer/usr/bin/arm-none-linux-gnueabi-gcc)
set(CMAKE_CXX_COMPILER /Users/niksun/Development/sdk/RaspberryPi.platform/Developer/usr/bin/arm-none-linux-gnueabi-g++)
set(MYSYSROOT /Users/niksun/Development/sdk/RaspberryPi.platform/Developer/SDKs/RaspberryPi7.sdk)

# compiler/linker flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot=${MYSYSROOT}" CACHE INTERNAL "" FORCE)
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} --sysroot=${MYSYSROOT}" CACHE INTERNAL "" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=${MYSYSROOT}" CACHE INTERNAL "" FORCE)
set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} --sysroot=${MYSYSROOT}" CACHE INTERNAL "" FORCE)
# cmake built-in settings to use find_xxx() functions
set(CMAKE_FIND_ROOT_PATH "${MYSYSROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(ALLOW_DUPLICATE_CUSTOM_TARGETS 1)
set(CMAKE_C_STANDARD 99)

add_subdirectory(process)
add_subdirectory(pthread)
add_subdirectory(select)
add_subdirectory(poll)
add_subdirectory(epoll)
