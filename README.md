# Batch Script Shell (BSS)

## Historical Context: Batch Files and Command Prompt

Batch files (.bat) and the Windows Command Prompt have their roots deeply embedded in computing history, stretching back to the early days of personal computing. The lineage traces back through the MS-DOS era of the 1980s, when Microsoft created MS-DOS (Microsoft Disk Operating System) as a command-line interface for early IBM PCs and compatibles.

The .bat file extension stands for "batch," referring to the ability to batch-process multiple commands that would otherwise need to be entered manually. These batch files contain sequences of commands that are executed in order, providing a means to automate repetitive tasks and complex procedures.

The design of MS-DOS was heavily influenced by earlier operating systems, most notably Digital Research's CP/M (Control Program/Monitor), which was one of the first operating systems for early personal computers in the 1970s. CP/M established many conventions that would later appear in MS-DOS, including the command-line interface, file system structure, and the concept of executable command files. Even the architecture of batch files - with their line-by-line command execution - can be traced back to these early systems.

The Windows Command Prompt (cmd.exe) that we know today evolved from these DOS roots, maintaining backward compatibility with many of the same commands and scripting capabilities. Commands like `dir`, `cd`, `echo`, and the general structure of batch files can be traced back through this lineage to CP/M. This heritage makes understanding DOS-era scripting essential for working with legacy build systems and batch scripts, particularly in development environments that have evolved from older systems.

## About BSS

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

### Using the Build Script

We've provided a convenient build script that wraps all build options:

```bash
# Build with default make
./build.sh build

# Build with CMake
./build.sh cmake

# Build with Autotools
./build.sh autotools

# Clean build artifacts
./build.sh clean

# Clean all including autotools generated files
./build.sh distclean

# Install the executable
./build.sh install

# Show help
./build.sh help
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