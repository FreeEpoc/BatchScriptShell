cmake_minimum_required(VERSION 3.10)
project(BatchScriptShell C)

set(CMAKE_C_STANDARD 99)

# Get git revision number
find_program(GIT git)
if(GIT)
    execute_process(
        COMMAND ${GIT} rev-list --count HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_REVISION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    # Make sure GIT_REVISION is a valid number
    if(NOT GIT_REVISION MATCHES "^[0-9]+$")
        set(GIT_REVISION 0)
    endif()
else()
    set(GIT_REVISION 0)
endif()

# Compiler flags with version definition
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g -DBSS_VERSION=\\\"r${GIT_REVISION}\\\"")

add_executable(bss shell.c)

install(TARGETS bss DESTINATION bin)