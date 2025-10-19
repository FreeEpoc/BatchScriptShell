#!/bin/bash

# Script to run all tests in the tests directory
# If any test fails (returns non-zero exit code), the script stops and returns a non-zero exit code
# 
# Note: The current implementation of the Batch Script Shell (BSS) may not propagate
# exit codes from commands within batch files correctly. For a test to register as
# failed, the shell process itself must return a non-zero exit code. Some improvements
# to the shell's error handling may be necessary to fully utilize this feature.

# Default values
stop_on_error=true
verbose=false

# Function to display usage
usage() {
    echo "Usage: $0 [OPTIONS]"
    echo "Options:"
    echo "  -h, --help              Show this help message"
    echo "  -c, --continue          Continue running tests even after a failure"
    echo "  -v, --verbose           Print detailed output for each test"
    echo "  -s, --stop-on-error     Stop on first error (default behavior)"
    exit 1
}

# Parse command-line options
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            usage
            ;;
        -c|--continue)
            stop_on_error=false
            shift
            ;;
        -v|--verbose)
            verbose=true
            shift
            ;;
        -s|--stop-on-error)
            stop_on_error=true
            shift
            ;;
        *)
            echo "Unknown option: $1"
            usage
            ;;
    esac
done

# Find the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TESTS_DIR="$SCRIPT_DIR/tests"

# Check if tests directory exists
if [[ ! -d "$TESTS_DIR" ]]; then
    echo "Error: Tests directory not found at $TESTS_DIR"
    exit 1
fi

# Find all .bat files in the tests directory
BAT_FILES=("$TESTS_DIR"/*.bat)

# Check if any .bat files were found
if [[ ! -e "${BAT_FILES[0]}" ]]; then
    echo "No .bat files found in $TESTS_DIR"
    exit 0
fi

# Initialize counters
total_tests=0
failed_tests=0
passed_tests=0

# Print initial message
echo "Running all batch script tests in $TESTS_DIR"
echo "============================================="

# Loop through each .bat file
for bat_file in "${BAT_FILES[@]}"; do
    # Check if the file exists (to handle case where no .bat files exist)
    if [[ ! -f "$bat_file" ]]; then
        continue
    fi
    
    # Extract just the filename for display
    test_name=$(basename "$bat_file")
    
    # Increment test counter
    ((total_tests++))
    
    if [[ "$verbose" == true ]]; then
        echo "Running test: $test_name"
    else
        echo -n "Running test: $test_name ... "
    fi
    
    # Execute the shell with the batch file
    # Using timeout to avoid hanging tests (set to 30 seconds)
    if timeout 30 "$SCRIPT_DIR/bss" "$bat_file" > /dev/null 2>&1; then
        # Test passed
        if [[ "$verbose" == true ]]; then
            echo "PASSED"
        else
            echo "PASSED"
        fi
        ((passed_tests++))
    else
        # Test failed
        exit_code=$?
        if [[ "$verbose" == true ]]; then
            echo "FAILED (exit code: $exit_code)"
        else
            echo "FAILED (exit code: $exit_code)"
        fi
        ((failed_tests++))
        
        # Print error message if not in continue mode
        if [[ "$stop_on_error" == true ]]; then
            echo "Test $test_name failed with exit code $exit_code. Stopping execution."
            echo "============================================="
            echo "Test Summary:"
            echo "  Total: $total_tests"
            echo "  Passed: $passed_tests"
            echo "  Failed: $failed_tests"
            exit $exit_code
        fi
    fi
done

# Print final results
echo "============================================="
echo "All tests completed."
echo "Test Summary:"
echo "  Total: $total_tests"
echo "  Passed: $passed_tests"
echo "  Failed: $failed_tests"

# Exit with appropriate code based on results
if [[ $failed_tests -gt 0 ]]; then
    echo "Some tests failed."
    exit 1
else
    echo "All tests passed!"
    exit 0
fi