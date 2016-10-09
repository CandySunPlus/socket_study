cmake_minimum_required (VERSION 2.8)
project (demo)
add_executable (client client.c config.h)
add_executable (server server.c config.h)
