# Batch Script Shell (BSS) - Development Documentation

## Build Systems

This project supports multiple build systems to accommodate different development environments and preferences.

### Current Build System (Makefile)

The project currently uses a simple Makefile-based build system with added support for CMake and autotools:

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = bss
SOURCES = shell.c

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

# CMake build target
cmake-build:
	mkdir -p build
	cd build && cmake ..
	$(MAKE) -C build

# Configure + Make (Autotools) support
configure: configure.ac
	autoreconf -fiv
	./configure

# Clean all build artifacts
clean:
	rm -f $(TARGET)
	rm -rf build/
	rm -rf autom4te.cache/
	if [ -f Makefile ] && [ -z "$MAKEFILE_GENERATED" ]; then rm -f config.log config.status Makefile; fi

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

.PHONY: clean install cmake-build configure
```

### CMake Build System

The project now supports CMake as an alternative build system. The CMakeLists.txt file:

```cmake
cmake_minimum_required(VERSION 3.10)
project(BatchScriptShell C)

set(CMAKE_C_STANDARD 99)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

add_executable(bss shell.c)

install(TARGETS bss DESTINATION bin)
```

To build with CMake:
```bash
mkdir build
cd build
cmake ..
make
```

### Configure + Make Build System (Autotools)

The project now supports the traditional autotools workflow:

- `make configure` - Generate configure script and configure the build environment
- `./configure && make` - Configure and build the executable
- `make install` - Install the executable

The autotools files include:
- configure.ac: Configuration script generator
- Makefile.am: Automake input file

Files for autotools:
configure.ac:
```
AC_INIT([bss], [1.0.0], [bug-report-address])
AM_INIT_AUTOMAKE([foreign])

AC_PROG_CC
AC_PROG_INSTALL

AC_CHECK_PROGS(MAKE, [make gmake])
if test -z "$MAKE"; then
   AC_MSG_ERROR([No make program found])
fi

AC_CONFIG_FILES([
   Makefile
])

AC_OUTPUT
```

Makefile.am:
```
bin_PROGRAMS = bss
bss_SOURCES = shell.c
bss_CFLAGS = -Wall -Wextra -std=c99 -g
```

This approach provides:
- Maximum portability across Unix-like systems
- Flexible configuration options
- Standard installation paths
- Dependency checking