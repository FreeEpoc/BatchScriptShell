# Test Runner Script for Batch Script Shell (BSS)

## Overview

The `run_all_tests.sh` script provides a comprehensive way to run all batch script tests in the `tests` directory with configurable error handling behavior.

## Features

- Runs all `.bat` files found in the `tests` directory
- Configurable error handling:
  - Stop on first failure (default)
  - Continue running all tests even after failures
- Command-line options for controlling execution
- Verbose output option for detailed test results
- Summary report with pass/fail counts

## Usage

```bash
./run_all_tests.sh [OPTIONS]
```

### Options

- `-h, --help`: Show help message
- `-c, --continue`: Continue running tests even after a failure
- `-v, --verbose`: Print detailed output for each test
- `-s, --stop-on-error`: Stop on first error (default behavior)

## Exit Codes

- `0`: All tests passed
- `1`: At least one test failed
- Other values: Specific test failure codes

## Implementation Notes

The script executes the BSS shell for each test file and checks the exit code. If a test's exit code is non-zero, it's considered a failure.

Note: The current implementation of the Batch Script Shell (BSS) may not propagate exit codes from commands within batch files correctly. For a test to register as failed, the shell process itself must return a non-zero exit code. Some improvements to the shell's error handling may be necessary to fully utilize this feature.

To make a test fail explicitly, use the `exit /b <code>` command in your batch script where `<code>` is a non-zero value.

## Examples

Run all tests, stopping at the first failure:
```bash
./run_all_tests.sh
```

Run all tests, continuing even after failures:
```bash
./run_all_tests.sh --continue
```

Run all tests with verbose output:
```bash
./run_all_tests.sh --verbose
```

Run all tests with both continue and verbose options:
```bash
./run_all_tests.sh --continue --verbose
```