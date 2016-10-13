cmake_minimum_required (VERSION 2.8)

add_executable (select_serv server.c ../config.h)
add_executable (select_cli client.c ../config.h)
