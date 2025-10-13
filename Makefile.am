# Get git revision number 
GIT_REVISION = $(shell git log --oneline | wc -l)
BSS_VERSION = r$(GIT_REVISION)

bin_PROGRAMS = bss
bss_SOURCES = shell.c
bss_CFLAGS = -Wall -Wextra -std=c99 -g -DBSS_VERSION=\"$(BSS_VERSION)\"