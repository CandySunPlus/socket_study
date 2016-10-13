cmake_minimum_required (VERSION 2.8)

add_executable (pthread_serv server.c ../config.h)
add_executable (pthread_cli client.c ../config.h)
