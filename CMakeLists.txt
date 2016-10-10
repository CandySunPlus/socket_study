cmake_minimum_required (VERSION 2.8)
project (socket)
add_executable (process_cli ./src/process/client.c ./src/config.h)
add_executable (process_serv ./src/process/server.c ./src/config.h)
