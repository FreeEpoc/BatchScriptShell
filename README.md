# Batch Script Shell (BSS)

The Batch Script Shell (BSS) is a C implementation of a shell that interprets basic batch commands used in Symbian build scripts, with support for Windows-style paths, environment variables, and basic command execution.

## Building

The project supports multiple build systems. Choose the one that best fits your environment:

### Using Default Make

```bash
make
```

To clean build artifacts:
```bash
make clean
```

### Using CMake

```bash
make cmake-build
```

Or manually:
```bash
mkdir build
cd build
cmake ..
make
```

### Using Autotools

```bash
make autotools-build
```

After the build completes, you'll need to restore the main Makefile:
```bash
make restore-main-makefile
```

Or manually:
```bash
autoreconf -fiv
./configure
make
```

To clean autotools-generated files:
```bash
make autotools-distclean
```

### Build Options

- **Default build**: Simple compilation using gcc directly
- **CMake**: Cross-platform build system with IDE support
- **Autotools**: Traditional Unix-style build with advanced configuration options

## Features

- Written in C
- Interpret basic batch commands used in Symbian build scripts
- Automatic conversion of Windows-style paths to Linux-style paths (`\\` to `/`)
- Execute Perl scripts called from batch files
- Support for environment variable handling
- Basic control flow operations (if statements, loops)
- Directory mounting capabilities (like in DOS)
- Support for different directory separators (`\\` and `/`)
- Environment variable `%OS%` that can be overridden

## Directory and Path Handling

- Shell defaults to `\\` directory separator but supports `/`
- Linux root directory `/` binds to `C:\\`
- Other directories can be mounted using the internal MOUNT command (similar to DOS)
- Environment variable `%OS%` is set to a value similar to `uname` in Linux, but can be overridden with program parameters and internal commands to values like "Windows_NT"

## Supported Commands

### Built-in Commands
- `echo` - Display messages or control command echoing
  - `echo off` - Turn off command echoing (in batch files)
  - `echo on` - Turn on command echoing (in batch files)
  - `echo message` - Display a message
- `set` - Display or set environment variables
- `cd` or `chdir` - Change current directory
- `pwd` - Print working directory
- `cls` or `clear` - Clear the screen
- `pause` - Wait for a keypress

### External Commands
The shell can execute external commands using the system's PATH.

## Usage

### Interactive Mode
```bash
./bss
```

### Batch File Execution
```bash
./bss <batch-file.bat>
```

## Implementation Notes

This shell is designed to work with Symbian build scripts and provides compatibility with common batch commands. It includes features like:

- Path conversion between Windows and Linux style
- Environment variable expansion (e.g., `%PATH%`)
- Batch file execution with support for `@echo off`
- Directory mounting similar to DOS SUBST command

## Approach

BSS focuses only on the subset of batch functionality actually used by the Symbian build system, rather than implementing full batch compatibility. This keeps the implementation minimal and maintainable while providing the necessary functionality for the build process.

The implementation includes:
- A command parser for basic batch syntax
- A command execution engine
- Path conversion utilities
- Directory mounting functionality
- Environment variable management
- Error handling and logging