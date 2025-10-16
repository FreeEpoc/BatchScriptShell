# BSS (Batch Script Shell) Test Scripts

This directory contains test batch scripts that demonstrate the DOS-like features of the Batch Script Shell (BSS).

## Test Scripts

### `dos_features_test.bat`
Tests all major DOS-like features implemented:
- FOR loops
- File commands: MD, RD, COPY, DEL
- Date and time commands
- DIR and TYPE commands
- FIND command
- SORT command
- Pipe functionality
- IF statements
- SET command with arithmetic
- Environment variables

### `pipe_test.bat`
Tests pipe functionality and related commands:
- TYPE | FIND
- TYPE | SORT
- TYPE | FIND /C
- Redirection combined with pipes
- FOR /F loops with file input

### `file_commands_test.bat`
Tests file system manipulation commands:
- MD (mkdir) and RD (rmdir)
- COPY and MOVE commands
- TYPE and DIR commands
- FIND command
- DEL command with multiple files

### `interactive_commands_test.bat`
Tests advanced features including:
- DATE and TIME commands
- Complex FOR loops
- SET command with arithmetic and assignment operators
- IF statements with comparisons and logical operators
- Environment variables and string operations

## How to Run Tests

Run any test script with BSS using:
```bash
./bss tests/dos_features_test.bat
```

## Expected Results

Each test script will verify that the implemented DOS-like functionality works as expected, with the shell behaving similarly to Windows command prompt for these features.