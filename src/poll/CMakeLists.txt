cmake_minimum_required (VERSION 2.8)

add_executable (poll_serv server.c ../config.h)
add_executable (poll_cli client.c ../config.h)
