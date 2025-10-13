cmake_minimum_required(VERSION 3.10)
project(BatchScriptShell C)

set(CMAKE_C_STANDARD 99)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

add_executable(bss shell.c)

install(TARGETS bss DESTINATION bin)