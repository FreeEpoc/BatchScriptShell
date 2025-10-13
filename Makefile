CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
# Get the git revision number as the commit count
GIT_REVISION := $(shell git log --oneline | wc -l)
# Add the revision as a compile-time definition
CFLAGS += -DBSS_VERSION=\"r$(GIT_REVISION)\"

TARGET = bss
SOURCES = shell.c

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

# CMake build target
cmake-build:
	mkdir -p build
	cd build && cmake ..
	$(MAKE) -C build

# Autotools build target - handles backup and restoration of Makefile
autotools-build:
	@echo "Building with autotools..."
	@cp Makefile Makefile.main-backup
	autoreconf -fiv
	./configure
	$(MAKE)
	@echo "Build complete. To restore main Makefile, run: make restore-main-makefile"

# Restore original Makefile
restore-main-makefile:
	@if [ -f Makefile.main-backup ]; then \
		cp Makefile.main-backup Makefile; \
		rm Makefile.main-backup; \
		echo "Main Makefile restored"; \
	else \
		echo "No backup found to restore"; \
	fi

# Clean basic build artifacts
clean:
	rm -f $(TARGET) *.o
	rm -rf build/

# Complete cleanup including autotools files
autotools-distclean: clean
	rm -rf autom4te.cache/ .deps/
	rm -f config.log config.status configure
	rm -f compile depcomp install-sh missing aclocal.m4 Makefile.in
	# Don't remove the main Makefile or its backup

# Install target
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

.PHONY: clean install cmake-build autotools-build restore-main-makefile autotools-distclean