#!/bin/bash

# Batch Script Shell (BSS) Build Script
# This script provides convenient commands for building BSS with different build systems

set -e  # Exit on any error

ACTION=${1:-build}

case "$ACTION" in
    build)
        echo "Building BSS with default Make..."
        make
        ;;
    cmake)
        echo "Building BSS with CMake..."
        make cmake-build
        ;;
    autotools)
        echo "Building BSS with Autotools..."
        make autotools-build
        ;;
    clean)
        echo "Cleaning build artifacts..."
        make clean
        ;;
    distclean)
        echo "Cleaning all build artifacts including autotools generated files..."
        make autotools-distclean
        ;;
    install)
        echo "Installing BSS..."
        make install
        ;;
    restore-makefile)
        echo "Restoring main Makefile..."
        make restore-main-makefile
        ;;
    help)
        echo "BSS Build Script"
        echo "Usage: $0 [action]"
        echo ""
        echo "Available actions:"
        echo "  build            - Build with default Make (default)"
        echo "  cmake            - Build with CMake"
        echo "  autotools        - Build with Autotools"
        echo "  clean            - Clean build artifacts"
        echo "  distclean        - Clean all including autotools generated files"
        echo "  install          - Install the executable"
        echo "  restore-makefile - Restore the main Makefile after autotools build"
        echo "  help             - Show this help message"
        ;;
    *)
        echo "Unknown action: $ACTION"
        echo "Run '$0 help' for available actions"
        exit 1
        ;;
esac