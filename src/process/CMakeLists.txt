cmake_minimum_required (VERSION 2.8)

add_executable (process_serv server.c ../config.h)
add_executable (process_cli client.c ../config.h)
