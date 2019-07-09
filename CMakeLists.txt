cmake_minimum_required(VERSION 3.14)
project(server C)

set(CMAKE_C_STANDARD 99)

add_executable(server daemon.c server.c server.h operation.c func.h)