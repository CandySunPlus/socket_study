cmake_minimum_required (VERSION 2.8)

add_executable (epoll_serv server.c ../config.h)
add_executable (epoll_cli client.c ../config.h)
