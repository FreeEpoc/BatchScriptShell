# BSS (Batch Script Shell) Test Scripts - Additional Tests

This directory contains additional test batch scripts based on the documentation in the docs/ directory. These tests complement the existing test suite to provide more comprehensive coverage of DOS-like commands.

## Test Scripts I've Created

### `echo_test.bat`
Tests the ECHO command functionality:
- Basic echo messages
- Echo with environment variables
- Echo on/off functionality
- Echo empty lines

### `cd_test.bat`
Tests the CD (Change Directory) command:
- Display current directory
- Change to root directory
- Change to parent directory
- Change using environment variables

### `copy_test.bat`
Tests the COPY command functionality:
- Copy file to same directory
- Copy file to different directory
- Copy all files with specific extension

### `del_test.bat`
Tests the DEL (Delete) command:
- Delete a single file
- Delete specific files in directory
- Delete all files with extension using wildcards

### `dir_test.bat`
Tests the DIR (Directory) command:
- Basic directory listing
- Directory listing with specific extension
- Recursive directory listing
- Brief format directory listing
- Directory listing with all file attributes

### `md_test.bat`
Tests the MD (Make Directory) command:
- Create a single directory
- Create nested directory structure
- Multiple directory creation

### `rd_test.bat`
Tests the RD (Remove Directory) command:
- Remove an empty directory
- Attempt to remove non-empty directory (should fail without /s)
- Remove non-empty directory with /s flag
- Remove directory with /q flag (quiet mode)

### `ren_test.bat`
Tests the REN (Rename) command:
- Rename a file
- Rename a directory
- Rename file with spaces in name

### `pause_test.bat`
Tests the PAUSE command:
- Basic pause functionality
- Pause in a loop

### `set_test.bat`
Tests the SET command:
- Display all environment variables
- Set a new environment variable
- Use SET with specific variable
- Set variable with spaces
- Arithmetic operations with SET /A

### `move_test.bat`
Tests the MOVE command:
- Move a single file
- Move all files with specific extension
- Move a directory

### `complex_test.bat`
Tests multiple commands in combination:
- Creates complex directory structure
- Uses multiple commands in sequence
- Demonstrates integration of various commands

### `additional_commands_test.bat`
Tests additional commands that might not be covered elsewhere:
- CLS (clear screen) command
- CHOICE command (with note about interactive mode)
- FIND command with various options
- TYPE command demonstration

### `run_all_tests.bat`
A convenience script to run all individual test scripts sequentially.

## How to Run Tests

Run any test script with BSS using:
```bash
./bss tests/echo_test.bat
```

Or run all tests using:
```bash
./bss tests/run_all_tests.bat
```

## Notes

These test scripts were created based on the documentation in the docs/ directory. They provide comprehensive coverage of the core batch commands, with particular attention to functionality mentioned in the documentation files.